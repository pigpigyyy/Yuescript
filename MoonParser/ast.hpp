#ifndef AST_HPP
#define AST_HPP


#include <cassert>
#include <list>
#include <stdexcept>
#include <type_traits>
#include "parser.hpp"


namespace parserlib {


class ast_node;
template <class T, bool OPT, bool MEM> class ast_ptr;
template <class T> class ast_list;
template <class T> class ast;


/** type of AST node stack.
 */
typedef std::vector<ast_node*> ast_stack;

extern int ast_type_id;

template<class T>
int ast_type() {
	static int type = ast_type_id++;
	return type;
}

enum class traversal {
	Continue,
	Return,
	Stop
};

/** Base class for AST nodes.
 */
class ast_node : public input_range {
public:
	ast_node() : _ref(0) {}

	void retain() {
		++_ref;
	}

	void release() {
		--_ref;
		if (_ref == 0) {
			delete this;
		}
	}

    /** interface for filling the contents of the node
        from a node stack.
        @param st stack.
     */
    virtual void construct(ast_stack& st) {}

    /** interface for visiting AST tree use.
     */
	virtual traversal traverse(const std::function<traversal (ast_node*)>& func);

	template <typename... Ts>
	struct select_last {
		using type = typename decltype((std::enable_if<true,Ts>{}, ...))::type;
	};
	template <typename... Ts>
	using select_last_t = typename select_last<Ts...>::type;

	template <class ...Args>
	select_last_t<Args...>* getByPath() {
		int types[] = {ast_type<Args>()...};
		return static_cast<select_last_t<Args...>*>(getByTypeIds(std::begin(types), std::end(types)));
	}

	virtual bool visitChild(const std::function<bool (ast_node*)>& func);

	virtual ast_node* getChild(int) const { return nullptr; }

	virtual size_t getChildCount() const { return 0; }

	virtual ast_node* getFirstChild() const { return nullptr; }

	virtual ast_node* getLastChild() const { return nullptr; }

	virtual size_t getId() const { return "ast_node"_id; }

	virtual const char* getName() const { return "ast_node"; }

	virtual int get_type() { return ast_type<ast_node>(); }
private:
	int _ref;
	ast_node* getByTypeIds(int* begin, int* end);
    template <class T, bool OPT, bool MEM> friend class ast_ptr;
    template <class ...Args> friend class ast_choice;
    template <class T> friend class ast_list;
    template <class T> friend class ast;
};

template<class T>
T* ast_cast(ast_node* node) {
	return node && ast_type<T>() == node->get_type() ? static_cast<T*>(node) : nullptr;
}

template<class T>
T* ast_to(ast_node* node) {
	assert(node->get_type() == ast_type<T>());
	return static_cast<T*>(node);
}

template <class ...Args>
bool ast_is(ast_node* node) {
	if (!node) return false;
	bool result = false;
	int type = node->get_type();
	using swallow = bool[];
	(void)swallow{result || (result = ast_type<Args>() == type)...};
	return result;
}

class ast_member;

/** type of ast member vector.
 */
typedef std::vector<ast_member*> ast_member_vector;


/** base class for AST nodes with children.
 */
class ast_container : public ast_node {
public:
    /** sets the container under construction to be this.
     */
    ast_container();

    /** returns the vector of AST members.
        @return the vector of AST members.
     */
    const ast_member_vector& members() const {
        return m_members;
    }

    /** Asks all members to construct themselves from the stack.
        The members are asked to construct themselves in reverse order.
        from a node stack.
        @param st stack.
     */
    virtual void construct(ast_stack& st) override;

	virtual traversal traverse(const std::function<traversal (ast_node*)>& func) override;

	virtual bool visitChild(const std::function<bool (ast_node*)>& func) override;

	virtual ast_node* getChild(int index) const override;

	virtual size_t getChildCount() const override;

	virtual ast_node* getFirstChild() const override;

	virtual ast_node* getLastChild() const override;

	virtual size_t getId() const override { return "ast_container"_id; }

	virtual const char* getName() const override { return "ast_container"; }
private:
    ast_member_vector m_members;

    friend class ast_member;
};


/** Base class for children of ast_container.
 */
class ast_member {
public:
    /** automatically registers itself to the container under construction.
     */
    ast_member(bool is_member) {
    	if (is_member) add_to_owner();
	}

    virtual ~ast_member() {}

    /** interface for filling the the member from a node stack.
        @param st stack.
     */
    virtual void construct(ast_stack& st) = 0;

    virtual bool accept(ast_node* node) = 0;

	virtual int get_type() { return ast_type<ast_member>(); }
private:
    //register the AST member to the current container.
    void add_to_owner();
};

template<class T>
T* ast_cast(ast_member* member) {
	return member && ast_type<T>() == member->get_type() ? static_cast<T*>(member) : nullptr;
}

class _ast_ptr : public ast_member {
public:
	_ast_ptr(ast_node* node, bool is_member) : ast_member(is_member), m_ptr(node) {
		if (node) node->retain();
	}

    virtual ~_ast_ptr() {
        if (m_ptr) {
        	m_ptr->release();
        	m_ptr = nullptr;
        }
    }

	ast_node* get() const {
		return m_ptr;
	}

	template <class T>
	T* as() const {
		return ast_cast<T>(m_ptr);
	}

	template <class T>
	T* to() const {
		assert(m_ptr->get_type() == ast_type<T>());
		return static_cast<T*>(m_ptr);
	}

	template <class T>
	bool is() const {
		return m_ptr->get_type() == ast_type<T>();
	}

	void set(ast_node* node) {
		if (node == m_ptr) {
			return;
		} else if (!node) {
			if (m_ptr) m_ptr->release();
			m_ptr = nullptr;
		} else {
			assert(accept(node));
			if (m_ptr) m_ptr->release();
			m_ptr = node;
			node->retain();
		}
	}

	virtual int get_type() override {
		return ast_type<_ast_ptr>();
	}
protected:
	ast_node* m_ptr;
};

/** pointer to an AST object.
    It assumes ownership of the object.
    It pops an object of the given type from the stack.
    @tparam T type of object to control.
    @tparam OPT if true, the object becomes optional.
 */
template <class T, bool OPT = false, bool MEM = true> class ast_ptr : public _ast_ptr {
public:
    ast_ptr(T* node = nullptr) : _ast_ptr(node, MEM) {}

    ast_ptr(const ast_ptr<T, OPT, MEM>& other) : _ast_ptr(other.get(), MEM) {}

    ast_ptr<T, OPT, MEM>& operator=(const ast_ptr<T, OPT, MEM>& other) {
    	set(other.get());
    	return *this;
	}

    /** gets the underlying ptr value.
        @return the underlying ptr value.
     */
    T* get() const {
        return static_cast<T*>(m_ptr);
    }

    /** auto conversion to the underlying object ptr.
        @return the underlying ptr value.
     */
    operator T*() const {
        return static_cast<T*>(m_ptr);
    }

    /** member access.
        @return the underlying ptr value.
     */
    T* operator->() const {
        assert(m_ptr);
        return static_cast<T*>(m_ptr);
    }

    /** Pops a node from the stack.
        @param st stack.
        @exception std::logic_error thrown if the node is not of the appropriate type;
            thrown only if OPT == false or if the stack is empty.
     */
    virtual void construct(ast_stack& st) override {
        //check the stack node
        if (st.empty()) {
			if (OPT) return;
			else throw std::logic_error("invalid AST stack");
		}

        ast_node* node = st.back();

		if (!ast_ptr::accept(node)) {
			//if the object is optional, simply return
			if (OPT) {
				return;
			} else { //else if the object is mandatory, throw an exception
				throw std::logic_error("invalid AST node");
			}
		}

        st.pop_back();

        m_ptr = node;
        node->retain();
    }
private:
    virtual bool accept(ast_node* node) override {
		return node && (std::is_same<ast_node,T>() || ast_type<T>() == node->get_type());
	}
};

template<class T>
inline ast_ptr<T, false, false> new_ptr() {
	return ast_ptr<T, false, false>(new T);
}

template <class ...Args> class ast_sel : public _ast_ptr {
public:
    ast_sel() : _ast_ptr(nullptr, true) {}

    ast_sel(const ast_sel<Args...>& other) : _ast_ptr(other.get(), true) {}

    ast_sel<Args...>& operator=(const ast_sel<Args...>& other) {
    	set(other.get());
    	return *this;
	}

    operator ast_node*() const {
        return m_ptr;
    }

    ast_node* operator->() const {
        assert(m_ptr);
        return m_ptr;
    }

    virtual void construct(ast_stack& st) override {
        if (st.empty()) {
			throw std::logic_error("invalid AST stack");
		}

        ast_node* node = st.back();

        if (!ast_sel::accept(node)) throw std::logic_error("invalid AST node");

        st.pop_back();

        m_ptr = node;
        node->retain();
    }
private:
    virtual bool accept(ast_node* node) override {
    	if (!node) return false;
		using swallow = bool[];
		bool result = false;
		(void)swallow{result || (result = ast_type<Args>() == node->get_type())...};
		return result;
	}
};

class _ast_list : public ast_member {
public:
    typedef std::list<ast_node*> container;

    _ast_list() : ast_member(true) {}

   ~_ast_list() {
        clear();
    }

	inline ast_node* back() const {
		return m_objects.back();
	}

	inline ast_node* front() const {
		return m_objects.front();
	}

	inline size_t size() const {
		return m_objects.size();
	}

	inline bool empty() const {
		return m_objects.empty();
	}

    void push_back(ast_node* node) {
    	assert(node && accept(node));
		m_objects.push_back(node);
		node->retain();
	}

    void push_front(ast_node* node) {
    	assert(node && accept(node));
		m_objects.push_front(node);
		node->retain();
	}

    void set_front(ast_node* node) {
    	assert(node && accept(node));
		m_objects.front()->release();
		m_objects.front() = node;
		node->retain();
	}

    void set_back(ast_node* node) {
    	assert(node && accept(node));
		m_objects.back()->release();
		m_objects.back() = node;
		node->retain();
	}

	 const container& objects() const {
        return m_objects;
    }

    void clear() {
        for(ast_node* obj : m_objects) {
            if (obj) obj->release();
        }
        m_objects.clear();
    }

    void dup(const _ast_list& src) {
        for(ast_node* obj : src.m_objects) {
            m_objects.push_back(obj);
            obj->retain();
        }
    }

	virtual int get_type() override { return ast_type<_ast_list>(); }
protected:
	container m_objects;
};

/** A list of objects.
    It pops objects of the given type from the ast stack, until no more objects can be popped.
    It assumes ownership of objects.
    @tparam T type of object to control.
 */
template <class T> class ast_list : public _ast_list {
public:
    ast_list() {}

    ast_list(const ast_list<T>& other) {
    	clear();
    	dup(other);
	}

    ast_list<T>& operator=(const ast_list<T>& other) {
    	clear();
    	dup(other);
    	return *this;
	}

    /** Pops objects of type T from the stack until no more objects can be popped.
        @param st stack.
     */
    virtual void construct(ast_stack &st) override {
        while (!st.empty()) {
            ast_node* node = st.back();

            //if the object was not not of the appropriate type,
            //end the list parsing
            if (!ast_list::accept(node)) return;

            st.pop_back();
            
            //insert the object in the list, in reverse order
            m_objects.push_front(node);
            node->retain();
        }
    }
private:
    virtual bool accept(ast_node* node) override {
		return node && (std::is_same<ast_node,T>() || ast_type<T>() == node->get_type());
	}
};

template <class ...Args> class ast_sel_list : public _ast_list {
public:
    ast_sel_list() {}

    ast_sel_list(const ast_sel_list<Args...>& other) {
    	clear();
    	dup(other);
	}

    ast_sel_list<Args...>& operator=(const ast_sel_list<Args...>& other) {
    	clear();
    	dup(other);
    	return *this;
	}

    /** Pops objects of type T from the stack until no more objects can be popped.
        @param st stack.
     */
    virtual void construct(ast_stack &st) override {
        while (!st.empty()) {
            ast_node* node = st.back();

            //if the object was not not of the appropriate type,
            //end the list parsing
            if (!ast_sel_list<Args...>::accept(node)) return;

            st.pop_back();

            //insert the object in the list, in reverse order
            m_objects.push_front(node);
            node->retain();
        }
    }
private:
    virtual bool accept(ast_node* node) override {
    	if (!node) return false;
		using swallow = bool[];
		bool result = false;
		(void)swallow{result || (result = ast_type<Args>() == node->get_type())...};
		return result;
	}
};

/** AST function which creates an object of type T
    and pushes it to the node stack.
 */
template <class T> class ast {
public:
    /** constructor.
        @param r rule to attach the AST function to.
     */
    ast(rule& r) {
        r.set_parse_proc(&_parse_proc);
    }

private:
    //parse proc
    static void _parse_proc(const pos& b, const pos& e, void* d) {
        ast_stack* st = reinterpret_cast<ast_stack*>(d);
        T* obj = new T;
        obj->m_begin = b;
        obj->m_end = e;
        obj->construct(*st);
        st->push_back(obj);
    }
};


/** parses the given input.
    @param i input.
    @param g root rule of grammar.
    @param el list of errors.
    @param ud user data, passed to the parse procedures.
    @return pointer to ast node created, or null if there was an error.
        The return object must be deleted by the caller.
 */
ast_node* _parse(input &i, rule &g, error_list &el, void* ud);


/** parses the given input.
    @param i input.
    @param g root rule of grammar.
    @param el list of errors.
    @param ud user data, passed to the parse procedures.
    @return ast nodes.
 */
template <class T> ast_ptr<T, false, false> parse(input &i, rule &g, error_list &el, void* ud = nullptr) {
    ast_node* node = _parse(i, g, el, ud);
    T* ast = ast_cast<T>(node);
    ast_ptr<T, false, false> ptr;
    if (ast) {
    	ast_stack st{node};
    	ptr.construct(st);
    } else if (node) {
    	delete node;
    }
	return ptr;
}


} //namespace parserlib


#endif //AST_HPP
