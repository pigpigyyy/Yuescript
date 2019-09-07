#include <cassert>
#include "ast.hpp"


namespace parserlib {


//current AST container.
static ast_container *_current = 0;

int ast_type_id = 0;

traversal ast_node::traverse(const std::function<traversal (ast_node*)>& func) {
	return func(this);
}

ast_node* ast_node::getByPath(std::initializer_list<std::size_t>) {
	return nullptr;
}

void ast_node::eachChild(const std::function<void (ast_node*)>&) { }

bool ast_node::visitChild(const std::function<bool (ast_node*)>&) {
	return false;
}

/** sets the container under construction to be this.
 */
ast_container::ast_container() {
    _current = this;
}


/** sets the container under construction to be this.
    @param src source object.
 */
ast_container::ast_container(const ast_container &src) {
    _current = this;
}

    
/** Asks all members to construct themselves from the stack.
    The members are asked to construct themselves in reverse order.
    from a node stack.
    @param st stack.
 */
void ast_container::construct(ast_stack &st) {
    for(ast_member_vector::reverse_iterator it = m_members.rbegin();
        it != m_members.rend();
        ++it)
    {
        ast_member *member = *it;
        member->construct(st);
    }
}

traversal ast_container::traverse(const std::function<traversal (ast_node*)>& func) {
	traversal action = func(this);
	switch (action) {
		case traversal::Stop: return traversal::Stop;
		case traversal::Return: return traversal::Continue;
		default: break;
	}
	const auto& members = this->members();
	for (auto member : members) {
		if (_ast_ptr* ptr = ast_cast<_ast_ptr>(member)) {
			if (ptr->get() && ptr->get()->traverse(func) == traversal::Stop) {
				return traversal::Stop;
			}
		} else if (_ast_list* list = ast_cast<_ast_list>(member)) {
			for (auto obj : list->objects()) {
				if (obj->traverse(func) == traversal::Stop) {
					return traversal::Stop;
				}
			}
		}
	}
	return traversal::Continue;
}

ast_node* ast_container::getByPath(std::initializer_list<std::size_t> paths) {
	ast_node* current = this;
	auto it = paths.begin();
	while (it != paths.end()) {
		ast_node* findNode = nullptr;
		current->visitChild([&](ast_node* node) {
			if (node->getId() == *it) {
				findNode = node;
				return true;
			}
			return false;
		});
		if (findNode) {
			current = findNode;
		} else {
			current = nullptr;
			break;
		}
		++it;
	}
	return current;
}

void ast_container::eachChild(const std::function<void (ast_node*)>& func) {
	const auto& members = this->members();
	for (auto member : members) {
		if (_ast_ptr* ptr = ast_cast<_ast_ptr>(member)) {
			if (ptr->get()) {
				func(ptr->get());
			}
		} else if (_ast_list* list = ast_cast<_ast_list>(member)) {
			for (auto obj : list->objects()) {
				if (obj) {
					func(obj);
				}
			}
		}
	}
}

bool ast_container::visitChild(const std::function<bool (ast_node*)>& func) {
	const auto& members = this->members();
	for (auto member : members) {
		if (_ast_ptr* ptr = ast_cast<_ast_ptr>(member)) {
			if (ptr->get()) {
				if (func(ptr->get())) return true;
			}
		} else if (_ast_list* list = ast_cast<_ast_list>(member)) {
			for (auto obj : list->objects()) {
				if (obj) {
					if (func(obj)) return true;
				}
			}
		}
	}
	return false;
}

ast_node* ast_container::getChild(int index) const {
	int i = 0;
	const auto& members = this->members();
	for (auto member : members) {
		if (_ast_ptr* ptr = ast_cast<_ast_ptr>(member)) {
			if (ptr->get()) {
				if (i == index) return ptr->get();
				i++;
			}
		} else if (_ast_list* list = ast_cast<_ast_list>(member)) {
			for (auto obj : list->objects()) {
				if (obj) {
					if (i == index) return obj;
					i++;
				}
			}
		}
	}
	return nullptr;
}

int ast_container::getChildCount() const {
	int count = 0;
	const auto& members = this->members();
	for (auto member : members) {
		if (_ast_ptr* ptr = ast_cast<_ast_ptr>(member)) {
			if (ptr->get()) count++;
		} else if (_ast_list* list = ast_cast<_ast_list>(member)) {
			for (auto obj : list->objects()) {
				if (obj) count++;
			}
		}
	}
	return count;
}

//register the AST member to the current container.
void ast_member::_init() {
    assert(_current);
    m_container = _current;
    _current->m_members.push_back(this);
}


/** parses the given input.
    @param i input.
    @param g root rule of grammar.
    @param el list of errors.
    @param ud user data, passed to the parse procedures.
    @return pointer to ast node created, or null if there was an error.
        The return object must be deleted by the caller.
 */
ast_node *parse(input &i, rule &g, error_list &el, void* ud) {
    ast_stack st;
    if (!parse(i, g, el, &st, ud)) return 0;
    assert(st.size() == 1);
    return st[0];
}


} //namespace parserlib
