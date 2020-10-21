$ ->
	import "moonp" as {:options}
	if options.tl_enabled
		options.target_extension = "tl"

macro expr to_lua = (codes)->
	"require('moonp').to_lua(#{codes}, reserve_line_number:false, same_module:true)"

macro expr trim = (name)->
	"if result = #{name}\\match '[\\'\"](.*)[\\'\"]' then result else #{name}"

export macro text var = (name, type, value = nil)->
	import "moonp" as {options:{:tl_enabled}}
	value = $to_lua(value)\gsub "^return ", ""
	if tl_enabled
		"local #{name}:#{$trim type} = #{value}", {name}
	else
		"local #{name} = #{value}", {name}

export macro text def = (name, type, value)->
	import "moonp" as {options:{:tl_enabled}}
	if tl_enabled
		value = $to_lua(value)\match "function%(.*%)(.*)end"
		"local function #{name}#{$trim type}\n#{value}\nend", {name}
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

export macro text field = (tab, sym, func, type, value)->
	import "moonp" as {options:{:tl_enabled}}
	if tl_enabled
		value = $to_lua(value)\match "^return function%(.-%)\n(.*)end"
		"function #{tab}#{$trim sym}#{func}#{$trim type}\n#{value}\nend"
	else
		value = $to_lua(value)\gsub "^return ", ""
		"#{tab}.#{func} = #{value}"

