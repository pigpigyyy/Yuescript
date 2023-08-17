-- testing `return` propagation

_ = -> _ = 变量x for 变量x in *物品
_ = -> [变量x for 变量x in *物品]


-- doesn't make sense on purpose
do
	for 变量x in *物品 do return 变量x

do
	return [变量x for 变量x in *物品]

do
	return {变量x,变量y for 变量x,变量y in *物品}

_ = ->
	if 变量a
		if 变量a
			变量a
		else
			变量b
	elseif 变量b
		if 变量a
			变量a
		else
			变量b
	else
		if 变量a
			变量a
		else
			变量b

do
	return if 变量a
		if 变量a
			变量a
		else
			变量b
	elseif 变量b
		if 变量a
			变量a
		else
			变量b
	else
		if 变量a
			变量a
		else
			变量b

do
	return
		:值
		物品A: 123
		物品B: "abc"

do
	return
		* 1
		* 2
		* ...三
		* 4

_ = -> 变量a\变量b
do 变量a\变量b

