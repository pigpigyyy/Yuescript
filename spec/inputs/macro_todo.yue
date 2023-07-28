export macro todoInner = (module, line, msg)->
	print "TODO#{msg and ': ' .. msg or ''} in file #{module}, at line #{line}"
	{
		code: "-- TODO#{msg and ': ' .. msg or ''}"
		type: "lua"
	}

export macro todo = (msg)->
	if msg
		"$todoInner $FILE, $LINE, #{msg}"
	else
		"$todoInner $FILE, $LINE"

