export macro 待办内部实现 = (模块, 行数, 消息)->
	print "待实现#{消息 and ': ' .. 消息 or ''}, 文件为: #{模块}, 代码行数: #{行数}"
	{
		code: "-- 待实现#{消息 and ': ' .. 消息 or ''}"
		type: "lua"
	}

export macro 待办 = (消息)->
	if 消息
		"$待办内部实现 $FILE, $LINE, #{消息}"
	else
		"$待办内部实现 $FILE, $LINE"

