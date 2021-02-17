export macro config = (debugging = true)->
	global debugMode = debugging == "true"
	global debugMacro = true
	""

export macro showMacro = (name,res)->
	if debugMacro
		"do
		txt = #{res}
		print '[macro '..#{name}..']'
		print txt
		txt"
	else
		res

export macro asserts = (cond)->
	if debugMode
		$showMacro "assert", "assert #{cond}"
	else
		""

export macro assert = (cond)->
	if debugMode
		$showMacro "assert", "assert #{cond}"
	else
		"#{cond}"

$config!

