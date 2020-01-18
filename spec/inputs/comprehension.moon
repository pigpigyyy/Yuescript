
-- see lists.moon for list comprehension tests

items = {1,2,3,4,5,6}
out = {k,k*2 for k in items}


x = hello: "world", okay: 2323

copy  = {k,v for k,v in pairs x when k != "okay"}

--

_ = { unpack(x) for x in yes }
_ = { unpack(x) for x in *yes }

_ = { xxxx for x in yes }
_ = { unpack [a*i for i, a in ipairs x] for x in *{{1,2}, {3,4}} }


--

n1 = [i for i=1,10]
n2 = [i for i=1,10 when i % 2 == 1]

aa = [{x,y} for x=1,10 for y=5,14]
bb = [y for thing in y for i=1,10]
cc = [y for i=1,10 for thing in y]
dd = [y for i=1,10 when cool for thing in y when x > 3 when c + 3]

_ = {"hello", "world" for i=1,10}

--

j = [a for {a,b,c} in things]
k = [a for {a,b,c} in *things]
i = [hello for {:hello, :world} in *things]

hj = {a,c for {a,b,c} in things}
hk = {a,c for {a,b,c} in *things}
hi = {hello,world for {:hello, :world} in *things}

ok(a,b,c) for {a,b,c} in things

--

_ = [item for item in *items[1 + 2,3+4]]
_ = [item for item in *items[hello! * 4, 2 - thing[4]]]



nil
