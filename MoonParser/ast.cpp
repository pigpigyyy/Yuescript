#include <cassert>
#include "ast.hpp"


namespace parserlib {


//current AST container.
static ast_container *_current = 0;

int ast_type_id = 0;


bool ast_node::visit(const std::function<bool (ast_node*)>& begin,
		const std::function<bool (ast_node*)>& end)
{
	return begin(this) || end(this);
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

bool ast_container::visit(const std::function<bool (ast_node*)>& begin,
		const std::function<bool (ast_node*)>& end)
{
	bool result = begin(this);
	if (result) return true;
	const auto& members = this->members();
	for (auto member : members) {
		if (_ast_ptr* ptr = ast_cast<_ast_ptr>(member)) {
			if (ptr->get() && ptr->get()->visit(begin, end)) {
				return true;
			}
		} else if (_ast_list* list = ast_cast<_ast_list>(member)) {
			for (auto obj : list->objects()) {
				if (obj->visit(begin, end)) {
					return true;
				}
			}
		}
	}
	return end(this);
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
