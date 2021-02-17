x = a
	.b
	.c
	--[[chain item]] .d

x
	.y = a
			.b
			\c!

func 1, arg2
	.value
	\get!, arg3
		.value
		\get!
	* 1
	* x?
		.y?
		.z?
	* 3

tbb =
	k1: a
		\b 123
		.c!
	k2:
		w1: a!
			.b
			\c!

tb = f1{}
	.a
	|> f2? "abc", _

f = -> [a
	.b
	\c 123 for {a} in *vals]

f1 = -> x, a
	\b 123
	.c "abc"

result = origin
	.transform.root
	.gameObject
	\Parents!
	\Descendants!
	\SelectEnable!
	\SelectVisible!
	\TagEqual "fx"
	\Where (x)->
		if x\IsTargeted!
			return false
		x.name\EndsWith "(Clone)"
	\Destroy!

origin.transform.root.gameObject
	\Parents!\Descendants!
	\SelectEnable!
	\SelectVisible!
	\TagEqual "fx"
	\Where (x)-> x.name\EndsWith "(Clone)"
	\Destroy!

with item
	.itemFieldA = 123

	\callMethod!\chainCall!

	\callMethod!
		\chainCall!
	\chainCall!

	switch .itemFieldB
		\getValue!
		when "Valid", \getItemState!
			\itemMethodA!\getValue!
		else
			\itemMethodB!
				\getValue!

	a = if .itemFieldC
		.itemFieldD
	else
		.itemFieldE

	for v in *values
		\itemMethodC v

	for i = 1, counter
		\itemMethodC i

	unless .b
		.c = while .itemFieldD
			\itemNext!
				\get!

nil
