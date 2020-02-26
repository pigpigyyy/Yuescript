
{"abc", 123, 998} |> foreach print

{1,2,3} \
	|> map((x)-> x * 2) \
	|> filter((x)-> x > 4) \
	|> reduce(0, (a,b)-> a + b) \
	|> print

[i |> tostring for i = 0,10] |> table.concat(",") |> print

b = 1 + 2 + (4 |> tostring |> print(1) or 123)

if x = 233 |> math.max 998
	print x

with b |> create? "new"
	.value = 123
	print \work!

123 |> f?

"abc" |> f1? |> f2?

c = "abc" |> f1? |> f2?

f = ->
	arg |> x.y?\if

998 |> func2 "abc", 233 |> func0 |> func1
998 |> func0("abc", 233) |> func1 |> func2

1 |> f 2, 3, 4, 5
val(2) |> f 1, _, 3, 4, 5
arr[3] |> f 1, 2, _, 4, 5

a = {"1","2","3"} |> table.concat("") |> tonumber |> f1(1, 2, 3, _) |> f2(1, _, 3)

do
	(x)<- map {1,2,3}
	x * 2

do
	(x)<- map _,{1,2,3}
	x * 2

do
	(x)<- filter _, do
		(x)<- map _,{1,2,3,4}
		x * 2
	x > 2

do
	(data)<- http?.get "ajaxtest"
	body[".result"]\html data
	(processed)<- http.post "ajaxprocess", data
	body[".result"]\append processed
	<- setTimeout 1000
	print "done"

do
	<- syncStatus
	(err,data="nil")<- loadAsync "file.moon"
	if err
		print err
		return
	(codes)<- compileAsync data
	func = loadstring codes
	func!

do
	<- f1
	<- f2
	do
		<- f3
		<- f4
	<- f5
	<- f6
	f7!

do
	{:result,:msg} = do
		(data)<- receiveAsync "filename.txt"
		print data
		(info)<- processAsync data
		check info
	print result,msg

	totalSize = (for file in *files
		(data)<- loadAsync file
		addToCache file,data) |> reduce 0,(a,b)-> a+b

propA = do
	(value)<= property => @_value
	print "old value: #{@_value}"
	print "new value: #{value}"
	@_value = value

propB = do
	<= property _, (value)=>
		print "old value: #{@_value}"
		print "new value: #{value}"
		@_value = value
	@_value

alert "hi"
