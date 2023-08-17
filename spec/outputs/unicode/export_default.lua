local _module_0 = nil
_u6253_u5370("没问题")
_module_0 = function()
	_u6253_u5370("你好")
	return 123
end
if not _u662f_u5426_u7ed3_u675f then
	_u51fd_u6570(123, "abc", function(_u53c2_u6570x, _u56de_u8c03y)
		_u6253_u5370(_u53c2_u6570x)
		return _u56de_u8c03y(function(_u53c2_u6570y, _u7ed3_u679c)
			if _u7ed3_u679c then
				return _u53d8_u91cfabc + _u53c2_u6570y
			else
				return _u53d8_u91cfabc
			end
		end)
	end)
end
return _module_0
