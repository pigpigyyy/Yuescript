local bind = grasp.bind;
(bind(stmt))({
	color = "Red"
})
local a = 'b'
local c = d;
(a(b))(c(d))
c = d.c;
(a(b))(c(d));
(c(d))(a(b))
local b
a, b = c, d;
(d(a))(c)
for i = 1, 10 do
	a = function() end
	(print)(1)
	a = f;
	(print)(2)
	if cond then
		(print)(3)
	end
	::abc::;
	(print)(4)
	goto abc;
	(print)(5)
end
do
	print(123)
end
return nil
