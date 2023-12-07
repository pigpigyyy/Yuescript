$ ->
	import "yue" as {:options}
	if options.tl_enabled
		options.target_extension = "tl"
	package.path ..= ";./spec/lib/?.lua"

macro to_lua = (code)->
	"require('yue').to_lua(#{code}, reserve_line_number:false, same_module:true)"

macro trim = (name)->
	"if result := #{name}\\match '[\\'\"](.*)[\\'\"]' then result else #{name}"

export macro local = (decl, value = nil)->
	import "yue" as {options:{:tl_enabled}}
	name, type = ($trim decl)\match "(.-):(.*)"
	if not (name and type)
		error "invalid local varaible declaration for \"#{decl}\""
	value = $to_lua(value)\gsub "^return ", ""
	code = if tl_enabled
		"local #{name}:#{$trim type} = #{value}"
	else
		"local #{name} = #{value}"
	{
		:code
		type: "text"
		locals: {name}
	}

export macro function = (decl, value)->
	import "yue" as {options:{:tl_enabled}}
	import "tl"
	decl = $trim decl
	name, type = decl\match "(.-)(%(.*)"
	if not (name and type)
		error "invalid function declaration for \"#{decl}\""
	tokens = tl.lex "function #{decl}"
	_, node = tl.parse_program tokens,{},"macro-function"
	args = table.concat [arg.tk for arg in *node[1].args],", "
	value = "(#{args})#{value}"
	code = if tl_enabled
		value = $to_lua(value)\match "function%([^\n]*%)(.*)end"
		"local function #{name}#{type}\n#{value}\nend"
	else
		value = $to_lua(value)\gsub "^return ", ""
		"local #{name} = #{value}"
	{
		:code
		type: "text"
		locals: {name}
	}

export macro record = (name, decl)->
	import "yue" as {options:{:tl_enabled}}
	code = if tl_enabled
		"local record #{name}
	#{decl}
end"
	else
		"local #{name} = {}"
	{
		:code
		type: "text"
		locals: {name}
	}

export macro method = (decl, value)->
	import "yue" as {options:{:tl_enabled}}
	import "tl"
	decl = $trim decl
	tab, sym, func, type = decl\match "(.-)([%.:])(.-)(%(.*)"
	if not (tab and sym and func and type)
		error "invalid method declaration for \"#{decl}\""
	tokens = tl.lex "function #{decl}"
	_, node = tl.parse_program tokens,{},"macro-function"
	args = table.concat [arg.tk for arg in *node[1].args],", "
	value = "(#{args})->#{value\match "[%-=]>(.*)"}"
	code = if tl_enabled
		value = $to_lua(value)\match "^return function%(.-%)\n(.*)end"
		"function #{tab}#{sym}#{func}#{type}\n#{value}\nend"
	else
		value = $to_lua(value)\gsub "^return ", ""
		"#{tab}.#{func} = #{value}"
	{
		:code
		type: "text"
	}

