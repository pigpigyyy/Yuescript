export macro block config = (debugging = true)->
	global debugMode = debugging == "true"
	global debugMacro = true
	""

export macro expr showMacro = (name,res)->
	if debugMacro
		"do
		txt = #{res}
		print '[macro '..#{name}..']'
		print txt
		txt"
	else
		"#{res}"

export macro block asserts = (cond)->
	if debugMode
		$showMacro "assert", "assert #{cond}"
	else
		""

export macro expr assert = (cond)->
	if debugMode
		$showMacro "assert", "assert #{cond}"
	else
		"#{cond}"

$config!

