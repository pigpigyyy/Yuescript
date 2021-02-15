x = a
	.b

x
	.y = a
			\b!

func 1, arg2
	.value
	\get!, arg3
		.value
		\get!

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

with item()
	.itemFieldA = 123
	item = x

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

	unless .b
		.c = while .itemFieldD
			\itemNext!
				\get!

nil
