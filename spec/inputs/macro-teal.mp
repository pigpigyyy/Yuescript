$ ->
	import "moonp" as {:options}
	if options.tl_enabled
		options.target_extension = "tl"
	package.path ..= "?.lua;./spec/lib/?.lua"

macro expr to_lua = (codes)->
	"require('moonp').to_lua(#{codes}, reserve_line_number:false, same_module:true)"

macro expr trim = (name)->
	"if result = #{name}\\match '[\\'\"](.*)[\\'\"]' then result else #{name}"

export macro text local = (decl, value = nil)->
	import "moonp" as {options:{:tl_enabled}}
	name, type = ($trim decl)\match "(.-):(.*)"
	if not (name and type)
		error "invalid local varaible declaration for \"#{decl}\""
	value = $to_lua(value)\gsub "^return ", ""
	if tl_enabled
		"local #{name}:#{$trim type} = #{value}", {name}
	else
		"local #{name} = #{value}", {name}

export macro text function = (decl, value)->
	import "moonp" as {options:{:tl_enabled}}
	import "tl"
	decl = $trim decl
	name, type = decl\match "(.-)(%(.*)"
	if not (name and type)
		error "invalid function declaration for \"#{decl}\""
	tokens = tl.lex "function #{decl}"
	_, node = tl.parse_program tokens,{},"macro-function"
	args = table.concat [arg.tk for arg in *node[1].args],", "
	value = "(#{args})#{value}"
	if tl_enabled
		value = $to_lua(value)\match "function%([^\n]*%)(.*)end"
		"local function #{name}#{type}\n#{value}\nend", {name}
	else
		value = $to_lua(value)\gsub "^return ", ""
		"local #{name} = #{value}", {name}

export macro text record = (name, decl)->
	import "moonp" as {options:{:tl_enabled}}
	if tl_enabled
		"local record #{name}
	#{decl}
end", {name}
	else
		"local #{name} = {}", {name}

export macro text method = (decl, value)->
	import "moonp" as {options:{:tl_enabled}}
	import "tl"
	decl = $trim decl
	tab, sym, func, type = decl\match "(.-)([%.:])(.-)(%(.*)"
	if not (tab and sym and func and type)
		error "invalid method declaration for \"#{decl}\""
	tokens = tl.lex "function #{decl}"
	_, node = tl.parse_program tokens,{},"macro-function"
	args = table.concat [arg.tk for arg in *node[1].args],", "
	value = "(#{args})->#{value\match "[%-=]>(.*)"}"
	if tl_enabled
		value = $to_lua(value)\match "^return function%(.-%)\n(.*)end"
		"function #{tab}#{sym}#{func}#{type}\n#{value}\nend"
	else
		value = $to_lua(value)\gsub "^return ", ""
		"#{tab}.#{func} = #{value}"

