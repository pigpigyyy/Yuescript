#include <cassert>
#include "ast.hpp"


namespace parserlib {

int ast_type_id = 0;

traversal ast_node::traverse(const std::function<traversal (ast_node*)>& func) {
	return func(this);
}

ast_node* ast_node::getByTypeIds(int* begin, int* end) {
	ast_node* current = this;
	auto it = begin;
	while (it != end) {
		ast_node* findNode = nullptr;
		int type = *it;
		current->visitChild([&](ast_node* node) {
			if (node->get_type() == type) {
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

bool ast_node::visitChild(const std::function<bool (ast_node*)>&) {
	return false;
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
        ast_member* member = *it;
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


/** parses the given input.
    @param i input.
    @param g root rule of grammar.
    @param el list of errors.
    @param ud user data, passed to the parse procedures.
    @return pointer to ast node created, or null if there was an error.
        The return object must be deleted by the caller.
 */
ast_node* _parse(input &i, rule &g, error_list &el, void* ud) {
    ast_stack st;
    if (!parse(i, g, el, &st, ud)) {
    	for (auto node : st) {
    		delete node;
		}
		st.clear();
    	return nullptr;
	}
    assert(st.size() == 1);
    return st.front();
}


} //namespace parserlib
