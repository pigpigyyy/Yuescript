local x = {
	val = 100,
	hello = function(self)
		return print(self.val)
	end
}
local fn
do
	local _base_0 = x
	local _fn_0 = _base_0.val
	fn = _fn_0 and function(...)
		return _fn_0(_base_0, ...)
	end
end
print(fn())
print(x:val());
(function(...)
	local _base_0 = hello(...)
	local _fn_0 = _base_0.world
	x = _fn_0 and function(...)
		return _fn_0(_base_0, ...)
	end
end)()
return nil
