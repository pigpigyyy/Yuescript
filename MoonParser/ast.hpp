#ifndef AST_HPP
#define AST_HPP


#include <cassert>
#include <list>
#include <stdexcept>
#include "parser.hpp"


namespace parserlib {


class ast_node;
template <class T, bool OPT> class ast_ptr;
template <class T> class ast_list;
template <class T> class ast;


/** type of AST node stack.
 */
typedef std::vector<ast_node *> ast_stack;


/** Base class for AST nodes.
 */
class ast_node : public input_range {
public:
    ///constructor.
    ast_node() : m_parent(0) {}
    
    /** copy constructor.
        @param n source object.
     */
    ast_node(const ast_node &n) : m_parent(0) {}

    ///destructor.
    virtual ~ast_node() {}
    
    /** assignment operator.
        @param n source object.
        @return reference to this.
     */
    ast_node &operator = (const ast_node &n) { return *this; }
    
    /** get the parent node.
        @return the parent node, if there is one.
     */
    ast_node *parent() const { return m_parent; }
    
    /** interface for filling the contents of the node
        from a node stack.
        @param st stack.
     */
    virtual void construct(ast_stack &st) {}
    
private:
    //parent
    ast_node *m_parent;    
    
    template <class T, bool OPT> friend class ast_ptr;
    template <class T> friend class ast_list;
    template <class T> friend class ast;
};


class ast_member;


/** type of ast member vector.
 */
typedef std::vector<ast_member *> ast_member_vector;


/** base class for AST nodes with children.
 */
class ast_container : public ast_node {
public:
    /** sets the container under construction to be this.
     */
    ast_container();

    /** sets the container under construction to be this.
        Members are not copied.
        @param src source object.
     */
    ast_container(const ast_container &src);

    /** the assignment operator.
        The members are not copied.
        @param src source object.
        @return reference to this.
     */
    ast_container &operator = (const ast_container &src) {
        return *this;
    }

    /** returns the vector of AST members.
        @return the vector of AST members.
     */
    const ast_member_vector &members() const {
        return m_members;
    }

    /** Asks all members to construct themselves from the stack.
        The members are asked to construct themselves in reverse order.
        from a node stack.
        @param st stack.
     */
    virtual void construct(ast_stack &st);

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
    ast_member() { _init(); }

    /** automatically registers itself to the container under construction.
        @param src source object.
     */
    ast_member(const ast_member &src) { _init(); }

    /** the assignment operator.
        @param src source object.
        @return reference to this.
     */
    ast_member &operator = (const ast_member &src) {
        return *this;
    }
    
    /** returns the container this belongs to.
        @return the container this belongs to.
     */
    ast_container *container() const { return m_container; }

    /** interface for filling the the member from a node stack.
        @param st stack.
     */
    virtual void construct(ast_stack &st) = 0;

private:
    //the container this belongs to.
    ast_container *m_container;

    //register the AST member to the current container.
    void _init();
};


/** pointer to an AST object.
    It assumes ownership of the object.
    It pops an object of the given type from the stack.
    @tparam T type of object to control.
    @tparam OPT if true, the object becomes optional.
 */
template <class T, bool OPT = false> class ast_ptr : public ast_member {
public:
    /** the default constructor.
        @param obj object.
     */
    ast_ptr(T *obj = 0) : m_ptr(obj) {
        _set_parent();
    }

    /** the copy constructor.
        It duplicates the underlying object.
        @param src source object.
     */
    ast_ptr(const ast_ptr<T, OPT> &src) :
        m_ptr(src.m_ptr ? new T(*src.m_ptr) : 0)
    {
        _set_parent();
    }

    /** deletes the underlying object.
     */
    ~ast_ptr() {
        delete m_ptr;
    }

    /** copies the given object.
        The old object is deleted.
        @param obj new object.
        @return reference to this.
     */
    ast_ptr<T, OPT> &operator = (const T *obj) {
        delete m_ptr;
        m_ptr = obj ? new T(*obj) : 0;
        _set_parent();
        return *this;
    }

    /** copies the underlying object.
        The old object is deleted.
        @param src source object.
        @return reference to this.
     */
    ast_ptr<T, OPT> &operator = (const ast_ptr<T, OPT> &src) {
        delete m_ptr;
        m_ptr = src.m_ptr ? new T(*src.m_ptr) : 0;
        _set_parent();
        return *this;
    }

    /** gets the underlying ptr value.
        @return the underlying ptr value.
     */
    T *get() const {
        return m_ptr;
    }

    /** auto conversion to the underlying object ptr.
        @return the underlying ptr value.
     */
    operator T *() const {
        return m_ptr;
    }

    /** member access.
        @return the underlying ptr value.
     */
    T *operator ->() const {
        assert(m_ptr);
        return m_ptr;
    }

    /** Pops a node from the stack.
        @param st stack.
        @exception std::logic_error thrown if the node is not of the appropriate type;
            thrown only if OPT == false or if the stack is empty.
     */
    virtual void construct(ast_stack &st) {
        //check the stack node
        if (st.empty()) throw std::logic_error("invalid AST stack");
    
        //get the node
        ast_node *node = st.back();
        
        //get the object
        T *obj = dynamic_cast<T *>(node);
        
        //if the object is optional, simply return
        if (OPT) {
            if (!obj) return;
        }
        
        //else if the object is mandatory, throw an exception
        else {
            if (!obj) throw std::logic_error("invalid AST node");
        }
        
        //pop the node from the stack
        st.pop_back();
        
        //set the new object
        delete m_ptr;
        m_ptr = obj;
        _set_parent();
    }

private:
    //ptr
    T *m_ptr;
    
    //set parent of object
    void _set_parent() {
        if (m_ptr) m_ptr->m_parent = container();
    }
};


/** A list of objects.
    It pops objects of the given type from the ast stack, until no more objects can be popped.
    It assumes ownership of objects.
    @tparam T type of object to control.
 */
template <class T> class ast_list : public ast_member {
public:
    ///list type.
    typedef std::list<T *> container;

    ///the default constructor.
    ast_list() {}

    /** duplicates the objects of the given list.
        @param src source object.
     */
    ast_list(const ast_list<T> &src) {
        _dup(src);
    }

    /** deletes the objects.
     */
    ~ast_list() {
        _clear();
    }

    /** deletes the objects of this list and duplicates the given one.
        @param src source object.
        @return reference to this.
     */
    ast_list<T> &operator = (const ast_list<T> &src) {
        if (&src != this) {
            _clear();
            _dup(src);
        }
        return *this;
    }

    /** returns the container of objects.
        @return the container of objects.
     */
    const container &objects() const {
        return m_objects;
    }

    /** Pops objects of type T from the stack until no more objects can be popped.
        @param st stack.
     */
    virtual void construct(ast_stack &st) {
        for(;;) {
            //if the stack is empty
            if (st.empty()) break;
            
            //get the node
            ast_node *node = st.back();
            
            //get the object
            T *obj = dynamic_cast<T *>(node);
            
            //if the object was not not of the appropriate type,
            //end the list parsing
            if (!obj) return;
            
            //remove the node from the stack
            st.pop_back();
            
            //insert the object in the list, in reverse order
            m_objects.push_front(obj);
            
            //set the object's parent
            obj->m_parent = ast_member::container();
        }
    }

private:
    //objects
    container m_objects;

    //deletes the objects of this list.
    void _clear() {
        while (!m_objects.empty()) {
            delete m_objects.back();
            m_objects.pop_back();
        }
    }

    //duplicate the given list.
    void _dup(const ast_list<T> &src) {
        for(typename container::const_iterator it = src.m_objects.begin();
            it != src.m_objects.end();
            ++it)
        {
            T *obj = new T(*it);
            m_objects.push_back(obj);
            obj->m_parent = ast_member::container();
        }
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
    ast(rule &r) {
        r.set_parse_proc(&_parse_proc);
    }

private:
    //parse proc
    static void _parse_proc(const pos &b, const pos &e, void *d) {
        ast_stack *st = reinterpret_cast<ast_stack *>(d);
        T *obj = new T;
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
ast_node *parse(input &i, rule &g, error_list &el, void* ud);


/** parses the given input.
    @param i input.
    @param g root rule of grammar.
    @param el list of errors.
    @param ast result pointer to created ast.
    @param ud user data, passed to the parse procedures.
    @return true on success, false on error.
 */
template <class T> bool parse(input &i, rule &g, error_list &el, T *&ast, void* ud = nullptr) {
    ast_node *node = parse(i, g, el, ud);
    ast = dynamic_cast<T *>(node);
    if (ast) return true;
    delete node;
    return false;
}


} //namespace parserlib


#endif //AST_HPP
