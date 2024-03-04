

x = -> print what

_ = ->

_ = -> -> ->

go to the barn

open -> the -> door

open ->
	the door
	hello = ->
		my func

h = -> hi

eat ->, world


(->)()

x = (...) ->

hello!
hello.world!

_ = hello!.something
_ = what!["ofefe"]

what! the! heck!

_ = (a,b,c,d,e) ->

_ = (a,a,a,a,a) ->
	print a

_ = (x=23023) ->

_ = (x=(y=()->) ->) ->

_ = (x = if something then yeah else no) ->

something = (hello=100, world=(x=[[yeah cool]])-> print "eat rice") ->
	print hello

_ = () =>
_ = (x, y) =>
_ = (@x, @y) =>
_ = (x=1) =>
_ = (@x=1,y,@z="hello world") =>


x -> return
y -> return 1
z -> return 1, "hello", "world"
k -> if yes then return else return

_ = -> real_name if something

--

d(
	->
		print "hello world"
	10
)



d(
	1,2,3
	4
	5
	6

	if something
		print "okay"
		10

	10,20
)


f(
	
	)(
	
	)(
		what
	)(->
		print "srue"
	123)

--

x = (a,
	b) ->
	 print "what"


y = (a="hi",
	b=23) ->
		print "what"

z = (
	a="hi",
	b=23) ->
		print "what"


j = (f,g,m,
	a="hi",
	b=23
) ->
		print "what"


y = (a="hi",
	b=23,
	...) ->
		print "what"


y = (a="hi",
	b=23,
	...
) ->
		print "what"

--

args = (a
	b) ->
		print "what"


args = (a="hi"
	b=23) ->
		print "what"

args = (
	a="hi"
	b=23) ->
		print "what"


args = (f,g,m
	a="hi"
	b=23
) ->
		print "what"


@ = (n)->
	return 1 if n == 0
	n * @(n-1)

do
	items.every (item) ->
		if item.field
			value = item.field.get "abc"
			if value
				switch value\get!
					when 123
						return false
					when 456
						handle item
		true

	items.every (item): true ->
		if item.field
			value = item.field.get "abc"
			if value
				switch value\get!
					when 123
						return false
					when 456
						-- prevent implicit return for next line
						handle item

	HttpServer\post "/login", (req): success: false ->
		switch req when {:name, :pwd}
			if name ~= ""
				if user := DB\queryUser name, pwd
					if user.status == "available"
						return success: true

	check = (num) -> return num
	-- func without implicit return
	func = (): -> check 123
	print func! -- get nil

nil
