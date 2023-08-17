do
	变量a = 0
	::开始::
	变量a += 1
	goto 结束 if 变量a == 5
	goto 开始
	::结束::

do
	for 计数z = 1, 10 do for 计数y = 1, 10 do for 计数x = 1, 10
		if 计数x^2 + 计数y^2 == 计数z^2
			打印 '找到了毕达哥拉斯三元组:', 计数x, 计数y, 计数z
			goto 完成
	::完成::

do
	for 计数z = 1, 10
		for 计数y = 1, 10 do for 计数x = 1, 10
			if 计数x^2 + 计数y^2 == 计数z^2
				打印 '找到了毕达哥拉斯三元组:', 计数x, 计数y, 计数z
				打印 'now trying next z...'
				goto 继续z
		::继续z::

do
	::重做::
	for 计数x = 1, 10 do for 计数y = 1, 10
		if not 函数f 计数x, 计数y then goto 继续
		if not 函数g 计数x, 计数y then goto 跳过
		if not 函数h 计数x, 计数y then goto 重做
		::继续::
	::跳过::

do
	for 变量 in *列表
		if 变量 % 2 == 0
			打印 '列表有偶数'
			goto 有
	打印 '列表没偶数'
	::有::

