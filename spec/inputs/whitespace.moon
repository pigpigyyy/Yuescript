
_ = {
	1, 2
}

_ = { 1, 2
}

_ = { 1, 2 }

_ = {1,2}

_ = {
1,2

}

_ = { something 1,2,
		4,5,6,
	3,4,5
}

_ = {
	a 1,2,3,
	4,5,6
	1,2,3
}


_ = {
	b 1,2,3,
		4,5,6
	1,2,3,
		1,2,3
}

_ = { 1,2,3 }

_ = { c 1,2,3,
}


hello 1,2,3,4,
	1,2,3,4,4,5

x 1,
	2, 3,
	4, 5, 6


hello 1,2,3,
	world 4,5,6,
		5,6,7,8

hello 1,2,3,
	world 4,5,6,
		5,6,7,8,
	9,9


_ = {
	hello 1,2,
	3,4,
	5, 6
}

x = {
	hello 1,2,3,4,
		5,6,7
	1,2,3,4
}

if hello 1,2,3,
	world,
	world
		print "hello"

if hello 1,2,3,
		world,
		world
	print "hello"


--

a(
	one, two, three
)

b(
	one,
	two,
	three
)


c(one, two,
	three, four)

--

v = ->
	a,
		b,
			c

v1, v2,
	v3 = ->
	a;
		b,
			c

a, b,
	c, d,
e, f = 1,
	f2
		:abc;
		3,
			4,
				f5 abc;
					6

for a,
		b,
			c in pairs tb
	print a,
			b,
			c

for i = 1,
	10,
	-1
	print i

local a,
				b,
					c

nil
