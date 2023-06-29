f((function()
	local _val_0 = -a ^ 2
	return 1 == _val_0 or 2 == _val_0 or 3 == _val_0
end)())
local a, b = (function(...)
	local _val_0 = x(...)
	return not (1 <= _val_0 and _val_0 <= 3)
end)(...), 2
local d
do
	local _val_0 = (tb.x.y(...))
	d = not (1 <= _val_0 and _val_0 <= 3)
end
local has
do
	local _val_0 = "foo"
	has = "bar" == _val_0 or "foo" == _val_0
end
if (1 == a) and (2 == b or 3 == b or 4 == b) or (function()
	local _val_0 = c
	return 1 <= _val_0 and _val_0 <= 10
end)() then
	print(a, b, c)
end
do
	local _exp_0 = val
	if 1 == _exp_0 or 2 == _exp_0 or 3 == _exp_0 then
		print("1, 2, 3")
	elseif not (0 < _exp_0 and _exp_0 <= 100) then
		print("not (0 < val <= 100)")
	elseif (200 <= _exp_0 and _exp_0 < 300) then
		print("200 <= val < 300)")
	elseif not (333 == _exp_0 or 444 == _exp_0 or 555 == _exp_0) then
		print("not 333, 444 or 555")
	end
end
do
	local _val_0 = y
	return not (a < _val_0 and _val_0 < b)
end
return nil
