local _module_0 = nil
print("OK")
_module_0 = function()
	print("hello")
	return 123
end
if not isOff then
	f(123, "abc", function(x, fy)
		print(x)
		return fy(function(y, res)
			if res then
				return abc + y
			else
				return abc
			end
		end)
	end)
end
return _module_0
