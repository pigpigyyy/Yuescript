
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

do
	(data) <- http?.get "ajaxtest"
	body[".result"]\html data
	(processed) <- http.post "ajaxprocess", data
	body[".result"]\append processed
	<- setTimeout 1000
	print "done"

do
	<- syncStatus
	(err,data="nil") <- loadAsync "file.moon"
	print err if err
	(codes) <- compileAsync data
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

alert "hi"
