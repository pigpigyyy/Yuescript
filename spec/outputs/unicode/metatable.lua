local _u53d8_u91cfa = setmetatable({
	["关闭"] = true,
}, {
	__close = function(self)
		return _u6253_u5370("离开作用域")
	end
})
local _u53d8_u91cfb = setmetatable({ }, {
	__add = function(_u5de6, _u53f3)
		return _u53f3 - _u5de6
	end
})
local _u53d8_u91cfc = setmetatable({
	["键1"] = true,
	["键2"] = true
}, {
	__add = add
})
local _u53d8_u91cfw = setmetatable({ }, {
	[_u540d_u79f0] = 123,
	["新建"] = function(self, _u503c)
		return {
			_u503c
		}
	end
})
getmetatable(_u53d8_u91cfw)["新建"](getmetatable(_u53d8_u91cfw)[_u540d_u79f0])
do
	local _ <close> = setmetatable({ }, {
		__close = function()
			return _u6253_u5370("离开作用域")
		end
	})
end
local _u53d8_u91cfd, _u53d8_u91cfe = _u53d8_u91cfa["关闭"], getmetatable(_u53d8_u91cfa).__close
local _u53d8_u91cff = getmetatable(_u53d8_u91cfa):__close(1)
getmetatable(_u53d8_u91cfa).__add = function(x, y)
	return x + y
end
do
	local _u65b0 = _u53d8_u91cfa["新"]
	local close, _u5173_u95edA
	do
		local _obj_0 = getmetatable(_u53d8_u91cfa)
		close, _u5173_u95edA = _obj_0.__close, _obj_0.__close
	end
	_u6253_u5370(_u65b0, close, _u5173_u95edA)
end
do
	local _u53d8_u91cfx, _u65b0, _u53d8_u91cf, close, _u5173_u95edA, num, add, sub
	do
		local _obj_0, _obj_1
		_u53d8_u91cfx, _obj_0, _obj_1 = 123, _u53d8_u91cfa["变量b"]["变量c"], _u51fd_u6570()
		_u65b0, _u53d8_u91cf = _obj_0["新"], _obj_0["变量"]
		do
			local _obj_2 = getmetatable(_obj_0)
			close, _u5173_u95edA = _obj_2.__close, _obj_2.__close
		end
		num = _obj_1.num
		do
			local _obj_2 = getmetatable(_obj_1)
			add, sub = _obj_2.__add, _obj_2.__sub
		end
	end
end
setmetatable(_u53d8_u91cfa["变量b"], { })
_u53d8_u91cfx.abc = 123
setmetatable(_u51fd_u6570(), mt)
setmetatable(_u53d8_u91cfb["变量c"], mt)
_u53d8_u91cfa, _u53d8_u91cfd, _u53d8_u91cfe = 1, "abc", nil
local _u76f8_u540c = getmetatable(_u53d8_u91cfa).__index == getmetatable(_u53d8_u91cfa).__index
setmetatable(_u53d8_u91cfa, {
	__index = _u8868
})
getmetatable(_u53d8_u91cfa).__index = _u8868
getmetatable(_u53d8_u91cfa).__index = _u8868
local mt = getmetatable(_u53d8_u91cfa)
local _call_0 = _u8868
_call_0["函数"](_call_0, #_u5217_u8868)
do
	local _obj_0 = getmetatable(_u8868)
	_obj_0["函数"](_obj_0, _u5217_u8868)
end
do
	local _obj_0 = getmetatable(_u8868)
	_obj_0["函数"](_obj_0, _u5217_u8868)
end
local index, _u8bbe_u7f6e_u51fd_u6570
do
	local _obj_0 = getmetatable(require("模块"))
	index, _u8bbe_u7f6e_u51fd_u6570 = _obj_0.__index, _obj_0.__newindex
end
do
	local _with_0 = _u8868
	_u6253_u5370(getmetatable(_with_0).__add, getmetatable(_with_0.x):__index("key"))
	do
		local _obj_0 = getmetatable(getmetatable(getmetatable(_with_0).__index).__add)
		_u53d8_u91cfa = _obj_0["新建"](_obj_0, 123)
	end
	_u53d8_u91cfb = t(#getmetatable(_with_0).__close["测试"])
	_u53d8_u91cfc = t(#getmetatable(_with_0).__close(_with_0["测试"]))
end
mt = getmetatable(_u53d8_u91cfa)
_u53d8_u91cfa = setmetatable({ }, mt)
_u53d8_u91cfa = setmetatable({ }, {
	__index = mt
})
local index
local _u7d22_u5f15 = getmetatable(_u53d8_u91cfa).__index
index = getmetatable(_u53d8_u91cfa).__index
do
	local _u6784_u9020, _u66f4_u65b0
	do
		local _obj_0 = getmetatable(_u53d8_u91cfa)
		_u6784_u9020, _u66f4_u65b0 = _obj_0["新"], _obj_0["更新"]
	end
end
do
	local _u6784_u9020, _u66f4_u65b0
	do
		local _obj_0 = getmetatable(_u53d8_u91cfa)
		_u6784_u9020, _u66f4_u65b0 = _obj_0["新"], _obj_0["更新"]
	end
end
local _u8868 = { }
do
	do
		local _obj_0 = getmetatable(_u8868)
		_u53d8_u91cff = _obj_0["值" .. tostring(x < y)](_obj_0, 123, ...)
	end
	_u53d8_u91cff((function(...)
		local _obj_0 = getmetatable(_u8868)
		return _obj_0['值'](_obj_0, 123, ...)
	end)(...))
	do
		local _obj_0 = getmetatable(_u8868)
		_obj_0[ [[		值
		1
	]]](_obj_0, 123, ...)
	end
	local _obj_0 = getmetatable(_u8868)
	return _obj_0["值" .. tostring(x > y)](_obj_0, 123, ...)
end
do
	do
		local _obj_0 = getmetatable(_u8868)
		_u53d8_u91cff = _obj_0['值'](_obj_0, 123, ...)
	end
	_u53d8_u91cff((function(...)
		local _obj_0 = getmetatable(_u8868)
		return _obj_0['值'](_obj_0, 123, ...)
	end)(...))
	do
		local _obj_0 = getmetatable(_u8868)
		_obj_0['值'](_obj_0, 123, ...)
	end
	local _obj_0 = getmetatable(_u8868)
	return _obj_0['值'](_obj_0, 123, ...)
end
do
	_u53d8_u91cff = getmetatable(_u8868)["值"](123, ...)
	_u53d8_u91cff = getmetatable(_u8868)["值" .. tostring(x < y)](123, ...)
	_u53d8_u91cff(getmetatable(_u8868)['值'](123, ...))
	getmetatable(_u8868)[ [[ 值
1]]](123, ...)
	return getmetatable(_u8868)["值" .. tostring(x > y)](123, ...)
end
return nil
