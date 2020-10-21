do
	close a, b = setmetatable {},__close:=> print "closed"
	const c, d = 123, 'abc'

	close a, b
	const c, d

do
	close v = if flag
		func!
	else
		setmetatable {},__close:=>

	close f = with io.open "file.txt"
		\write "Hello"

macro block defer = (item)-> "close _ = #{item}"
macro block defer_f = (func)-> "close _ = setmetatable {},__close:#{func}"

do
	$defer with io.open "file.txt"
		\write "Hello"
	
	$defer setmetatable {},__close:=> print "second"

	$defer_f -> print "first"

_defers = setmetatable {},__close:=>
	@[#@]!
	@[#@] = nil

macro block defer_i = (item)-> "
_defers[#_defers + 1] = #{item}
close _ = _defers"

do
	$defer_i -> print 3
	$defer_i -> print 2
	$defer_i -> print 1

