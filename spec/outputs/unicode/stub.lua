local _u53d8_u91cfx = {
	["值"] = 100,
	["你好"] = function(self)
		return _u6253_u5370(self.val)
	end
}
local _u51fd_u6570
do
	local _base_0 = _u53d8_u91cfx
	local _fn_0 = _base_0["值"]
	_u51fd_u6570 = _fn_0 and function(...)
		return _fn_0(_base_0, ...)
	end
end
_u6253_u5370(_u51fd_u6570())
_u6253_u5370(_u53d8_u91cfx["值"](_u53d8_u91cfx));
(function(...)
	do
		local _base_0 = _u4f60_u597d(...)
		local _fn_0 = _base_0["世界"]
		_u53d8_u91cfx = _fn_0 and function(...)
			return _fn_0(_base_0, ...)
		end
	end
end)()
return nil
