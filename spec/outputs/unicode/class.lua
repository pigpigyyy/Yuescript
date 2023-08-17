local _u4f60_u597d
do
	local _class_0
	local _base_0 = {
		["你好"] = function(self)
			return _u6253_u5370(self["测试"], self["世界"])
		end,
		__tostring = function(self)
			return "你好 世界"
		end
	}
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function(self, _u6d4b_u8bd5, _u4e16_u754c)
			self["测试"] = _u6d4b_u8bd5
			self["世界"] = _u4e16_u754c
			return _u6253_u5370("创建对象..")
		end,
		__base = _base_0,
		__name = "你好"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	_u4f60_u597d = _class_0
end
local _u5bf9_u8c61x = _u4f60_u597d(1, 2)
_u5bf9_u8c61x["你好"](_u5bf9_u8c61x)
_u6253_u5370(_u5bf9_u8c61x)
local _u7b80_u5355
do
	local _class_0
	local _base_0 = {
		["酷"] = function(self)
			return _u6253_u5370("酷")
		end
	}
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "简单"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	_u7b80_u5355 = _class_0
end
local _u4f0a_u514b_u65af
do
	local _class_0
	local _parent_0 = _u7b80_u5355
	local _base_0 = { }
	for _key_0, _val_0 in pairs(_parent_0.__base) do
		if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
			_base_0[_key_0] = _val_0
		end
	end
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	setmetatable(_base_0, _parent_0.__base)
	_class_0 = setmetatable({
		__init = function(self)
			return _u6253_u5370("你好已创建")
		end,
		__base = _base_0,
		__name = "伊克斯",
		__parent = _parent_0
	}, {
		__index = function(cls, name)
			local val = rawget(_base_0, name)
			if val == nil then
				local parent = rawget(cls, "__parent")
				if parent then
					return parent[name]
				end
			else
				return val
			end
		end,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	if _parent_0.__inherited then
		_parent_0.__inherited(_parent_0, _class_0)
	end
	_u4f0a_u514b_u65af = _class_0
end
local x_u5bf9_u8c61 = _u4f0a_u514b_u65af()
x_u5bf9_u8c61["酷"](x_u5bf9_u8c61)
local _u55e8
do
	local _class_0
	local _base_0 = {
		["酷"] = function(self, _u6570_u503c)
			return _u6253_u5370("数值", _u6570_u503c)
		end
	}
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function(self, _u53c2_u6570)
			return _u6253_u5370("初始化参数", _u53c2_u6570)
		end,
		__base = _base_0,
		__name = "嗨"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	_u55e8 = _class_0
end
do
	local _class_0
	local _parent_0 = _u55e8
	local _base_0 = {
		["酷"] = function(self)
			return _class_0.__parent.__base["酷"](self, 120302)
		end
	}
	for _key_0, _val_0 in pairs(_parent_0.__base) do
		if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
			_base_0[_key_0] = _val_0
		end
	end
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	setmetatable(_base_0, _parent_0.__base)
	_class_0 = setmetatable({
		__init = function(self)
			return _class_0.__parent.__init(self, "伙计")
		end,
		__base = _base_0,
		__name = "简单",
		__parent = _parent_0
	}, {
		__index = function(cls, name)
			local val = rawget(_base_0, name)
			if val == nil then
				local parent = rawget(cls, "__parent")
				if parent then
					return parent[name]
				end
			else
				return val
			end
		end,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	if _parent_0.__inherited then
		_parent_0.__inherited(_parent_0, _class_0)
	end
	_u7b80_u5355 = _class_0
end
x_u5bf9_u8c61 = _u7b80_u5355()
x_u5bf9_u8c61["酷"](x_u5bf9_u8c61)
_u6253_u5370(x_u5bf9_u8c61.__class == _u7b80_u5355)
local _u597d_u5427
do
	local _class_0
	local _base_0 = {
		["一些东西"] = 20323
	}
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "好吧"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	_u597d_u5427 = _class_0
end
local _u597d_u54d2
do
	local _class_0
	local _parent_0 = _u597d_u5427
	local _base_0 = {
		["一些东西"] = function(self)
			_class_0.__parent.__base["一些东西"](self, 1, 2, 3, 4)
			_class_0.__parent["一些东西"](_u53e6_u4e00_u4e2a_u81ea_u5df1, 1, 2, 3, 4)
			return _u65ad_u8a00(_class_0.__parent == _u597d_u5427)
		end
	}
	for _key_0, _val_0 in pairs(_parent_0.__base) do
		if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
			_base_0[_key_0] = _val_0
		end
	end
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	setmetatable(_base_0, _parent_0.__base)
	_class_0 = setmetatable({
		__init = function(self, ...)
			return _class_0.__parent.__init(self, ...)
		end,
		__base = _base_0,
		__name = "好哒",
		__parent = _parent_0
	}, {
		__index = function(cls, name)
			local val = rawget(_base_0, name)
			if val == nil then
				local parent = rawget(cls, "__parent")
				if parent then
					return parent[name]
				end
			else
				return val
			end
		end,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	if _parent_0.__inherited then
		_parent_0.__inherited(_parent_0, _class_0)
	end
	_u597d_u54d2 = _class_0
end
local _u597d
do
	local _class_0
	local _base_0 = {
		["不错"] = function(self)
			local _call_0 = _class_0.__parent
			return _call_0["一些东西"](_call_0, self, 1, 2, 3, 4)
		end
	}
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "好"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	_u597d = _class_0
end
local _u4ec0_u4e48
do
	local _class_0
	local _base_0 = {
		["一些东西"] = function(self)
			return _u6253_u5370("值:", self["值"])
		end
	}
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "什么"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	_u4ec0_u4e48 = _class_0
end
do
	local _class_0
	local _parent_0 = _u4ec0_u4e48
	local _base_0 = {
		["值"] = 2323,
		["一些东西"] = function(self)
			local _base_1 = _class_0.__parent
			local _fn_0 = _base_1["一些东西"]
			return _fn_0 and function(...)
				return _fn_0(_base_1, ...)
			end
		end
	}
	for _key_0, _val_0 in pairs(_parent_0.__base) do
		if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
			_base_0[_key_0] = _val_0
		end
	end
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	setmetatable(_base_0, _parent_0.__base)
	_class_0 = setmetatable({
		__init = function(self, ...)
			return _class_0.__parent.__init(self, ...)
		end,
		__base = _base_0,
		__name = "你好",
		__parent = _parent_0
	}, {
		__index = function(cls, name)
			local val = rawget(_base_0, name)
			if val == nil then
				local parent = rawget(cls, "__parent")
				if parent then
					return parent[name]
				end
			else
				return val
			end
		end,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	if _parent_0.__inherited then
		_parent_0.__inherited(_parent_0, _class_0)
	end
	_u4f60_u597d = _class_0
end
do
	local _with_0 = _u4f60_u597d()
	_u5bf9_u8c61x = _with_0["一些对象"](_with_0)
	_u6253_u5370(_u5bf9_u8c61x)
	_u5bf9_u8c61x()
end
local _u8d85_u7ea7_u9177
do
	local _class_0
	local _base_0 = {
		["👋"] = function(self)
			_class_0.__parent.__base["👋"](self, 1, 2, 3, 4)(1, 2, 3, 4)
			_class_0.__parent["一些东西"](1, 2, 3, 4)
			local _ = _class_0.__parent["一些东西"](1, 2, 3, 4)["世界"]
			local _call_0 = _class_0.__parent
			_call_0["好吧"](_call_0, self, "世界")["不错"](_u54c8, _u54c8, _u54c8)
			_ = _u4e00_u4e9b_u4e1c_u897f["上级"]
			_ = _class_0.__parent["上级"]["上级"]["上级"]
			do
				local _base_1 = _class_0.__parent
				local _fn_0 = _base_1["你好"]
				_ = _fn_0 and function(...)
					return _fn_0(_base_1, ...)
				end
			end
			return nil
		end
	}
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "超级酷"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	_u8d85_u7ea7_u9177 = _class_0
end
local _u53d8_u91cfx = self["你好"]
_u53d8_u91cfx = self.__class["你好"]
self["你好"](self, "世界")
self.__class["你好"](self.__class, "世界")
self.__class["一"](self.__class, self.__class["二"](self.__class, 4, 5)(self["三"], self["四"]))
local _u53d8_u91cfxx
_u53d8_u91cfxx = function(_u4f60_u597d, _u4e16_u754c, _u9177)
	self["你好"] = _u4f60_u597d
	self.__class["世界"] = _u4e16_u754c
end
local _u4e00_u4e2a_u7c7b
do
	local _class_0
	local _base_0 = {
		["蓝色"] = function(self) end,
		["绿色"] = function(self) end
	}
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "一个类"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	local self = _class_0;
	self["好"] = 343
	self["你好"] = 3434
	self["世界"] = 23423
	self["红色"] = function(self) end
	_u4e00_u4e2a_u7c7b = _class_0
end
_u53d8_u91cfx = self
local _u53d8_u91cfy = self.__class
self(_u4e00_u4e9b_u4e1c_u897f)
self.__class(_u4e00_u4e9b_u4e1c_u897f)
local self = self + self / self
self = 343
self["你好"](2, 3, 4)
local _ = _u4f60_u597d[self]["世界"]
local _u602a_u602a_u7684
do
	local _class_0
	local _base_0 = { }
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "怪怪的"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	local self = _class_0;
	_ = self["你好"]
	if _u4e00_u4e9b_u4e1c_u897f then
		_u6253_u5370("你好世界")
	end
	_u4f60_u597d = "世界"
	self["另一"] = "天"
	if _u4e00_u4e9b_u4e1c_u897f then
		_u6253_u5370("好")
	end
	_u602a_u602a_u7684 = _class_0
end
_u6253_u5370("你好")
local _u53d8_u91cfyyy
_u53d8_u91cfyyy = function()
	local _u9177
	do
		local _class_0
		local _base_0 = { }
		if _base_0.__index == nil then
			_base_0.__index = _base_0
		end
		_class_0 = setmetatable({
			__init = function() end,
			__base = _base_0,
			__name = "酷"
		}, {
			__index = _base_0,
			__call = function(cls, ...)
				local _self_0 = setmetatable({ }, _base_0)
				cls.__init(_self_0, ...)
				return _self_0
			end
		})
		_base_0.__class = _class_0
		local self = _class_0;
		_ = nil
		_u9177 = _class_0
		return _class_0
	end
end
do
	local _class_0
	local _base_0 = { }
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "子类D"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	local self = _class_0;
	_ = nil
	_u5bf9_u8c61a["字段b"]["字段c"]["子类D"] = _class_0
end
do
	local _class_0
	local _base_0 = { }
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "你好"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	local self = _class_0;
	_ = nil
	_u5bf9_u8c61a["字段b"]["你好"] = _class_0
end
do
	local _class_0
	local _parent_0 = _u4f60_u597d["世界"]
	local _base_0 = { }
	for _key_0, _val_0 in pairs(_parent_0.__base) do
		if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
			_base_0[_key_0] = _val_0
		end
	end
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	setmetatable(_base_0, _parent_0.__base)
	_class_0 = setmetatable({
		__init = function(self, ...)
			return _class_0.__parent.__init(self, ...)
		end,
		__base = _base_0,
		__name = "某个成员",
		__parent = _parent_0
	}, {
		__index = function(cls, name)
			local val = rawget(_base_0, name)
			if val == nil then
				local parent = rawget(cls, "__parent")
				if parent then
					return parent[name]
				end
			else
				return val
			end
		end,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	local self = _class_0;
	_ = nil
	if _parent_0.__inherited then
		_parent_0.__inherited(_parent_0, _class_0)
	end
	(function()
		return require("moon")
	end)()["某个成员"] = _class_0
end
local _u7c7ba
do
	local _class_0
	local _base_0 = { }
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "类a"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	_u7c7ba = _class_0
end
local _u7c7bb
do
	local _class_0
	local _base_0 = { }
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "一个类"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	_u4e00_u4e2a_u7c7b = _class_0
	_u7c7bb = _class_0
end
local _u7c7bc
do
	local _class_0
	local _parent_0 = _u4f60_u597d
	local _base_0 = { }
	for _key_0, _val_0 in pairs(_parent_0.__base) do
		if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
			_base_0[_key_0] = _val_0
		end
	end
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	setmetatable(_base_0, _parent_0.__base)
	_class_0 = setmetatable({
		__init = function(self, ...)
			return _class_0.__parent.__init(self, ...)
		end,
		__base = _base_0,
		__name = "一个类",
		__parent = _parent_0
	}, {
		__index = function(cls, name)
			local val = rawget(_base_0, name)
			if val == nil then
				local parent = rawget(cls, "__parent")
				if parent then
					return parent[name]
				end
			else
				return val
			end
		end,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	if _parent_0.__inherited then
		_parent_0.__inherited(_parent_0, _class_0)
	end
	_u4e00_u4e2a_u7c7b = _class_0
	_u7c7bc = _class_0
end
local _u7c7bd
do
	local _class_0
	local _parent_0 = _u4e16_u754c
	local _base_0 = { }
	for _key_0, _val_0 in pairs(_parent_0.__base) do
		if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
			_base_0[_key_0] = _val_0
		end
	end
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	setmetatable(_base_0, _parent_0.__base)
	_class_0 = setmetatable({
		__init = function(self, ...)
			return _class_0.__parent.__init(self, ...)
		end,
		__base = _base_0,
		__name = "类d",
		__parent = _parent_0
	}, {
		__index = function(cls, name)
			local val = rawget(_base_0, name)
			if val == nil then
				local parent = rawget(cls, "__parent")
				if parent then
					return parent[name]
				end
			else
				return val
			end
		end,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	if _parent_0.__inherited then
		_parent_0.__inherited(_parent_0, _class_0)
	end
	_u7c7bd = _class_0
end
_u6253_u5370(((function()
	local _u5565_u4e8b
	do
		local _class_0
		local _base_0 = { }
		if _base_0.__index == nil then
			_base_0.__index = _base_0
		end
		_class_0 = setmetatable({
			__init = function() end,
			__base = _base_0,
			__name = "啥事"
		}, {
			__index = _base_0,
			__call = function(cls, ...)
				local _self_0 = setmetatable({ }, _base_0)
				cls.__init(_self_0, ...)
				return _self_0
			end
		})
		_base_0.__class = _class_0
		_u5565_u4e8b = _class_0
		return _class_0
	end
end)()).__name)
do
	local _class_0
	local _base_0 = { }
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "一个类"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	local self = _class_0;
	_ = nil
	_u4e00_u4e2a_u7c7b = _class_0
end
do
	local _class_0
	local _u503c, _u63d2_u5165
	local _base_0 = { }
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function(self)
			return _u6253_u5370(_u63d2_u5165, _u503c)
		end,
		__base = _base_0,
		__name = "一个类"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	local self = _class_0;
	_u503c = 23
	_u63d2_u5165 = _u8868["插入"]
	_u4e00_u4e2a_u7c7b = _class_0
end
do
	local _class_0
	local _base_0 = { }
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = _u55e8,
		__base = _base_0,
		__name = "X类"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	X_u7c7b = _class_0
end
local _u9177
do
	local _class_0
	local _parent_0 = _u4e1c_u897f
	local _base_0 = {
		["当"] = function(self)
			return {
				["你好"] = function()
					return _class_0.__parent.__base["当"](self)
				end,
				["世界"] = function()
					return _class_0.__parent.one
				end
			}
		end
	}
	for _key_0, _val_0 in pairs(_parent_0.__base) do
		if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
			_base_0[_key_0] = _val_0
		end
	end
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	setmetatable(_base_0, _parent_0.__base)
	_class_0 = setmetatable({
		__init = function(self, ...)
			return _class_0.__parent.__init(self, ...)
		end,
		__base = _base_0,
		__name = "酷",
		__parent = _parent_0
	}, {
		__index = function(cls, name)
			local val = rawget(_base_0, name)
			if val == nil then
				local parent = rawget(cls, "__parent")
				if parent then
					return parent[name]
				end
			else
				return val
			end
		end,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	if _parent_0.__inherited then
		_parent_0.__inherited(_parent_0, _class_0)
	end
	_u9177 = _class_0
end
local _u5947_u602a
do
	local _class_0
	local _parent_0 = _u4e1c_u897f
	local _base_0 = {
		["当"] = _u505a_u70b9_u4e8b(function(self)
			return _class_0.__parent.__base["当"](self)
		end)
	}
	for _key_0, _val_0 in pairs(_parent_0.__base) do
		if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
			_base_0[_key_0] = _val_0
		end
	end
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	setmetatable(_base_0, _parent_0.__base)
	_class_0 = setmetatable({
		__init = function(self, ...)
			return _class_0.__parent.__init(self, ...)
		end,
		__base = _base_0,
		__name = "奇怪",
		__parent = _parent_0
	}, {
		__index = function(cls, name)
			local val = rawget(_base_0, name)
			if val == nil then
				local parent = rawget(cls, "__parent")
				if parent then
					return parent[name]
				end
			else
				return val
			end
		end,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	if _parent_0.__inherited then
		_parent_0.__inherited(_parent_0, _class_0)
	end
	_u5947_u602a = _class_0
end
local _u5594_u54c8
do
	local _class_0
	local _parent_0 = _u4e1c_u897f
	local _base_0 = { }
	for _key_0, _val_0 in pairs(_parent_0.__base) do
		if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
			_base_0[_key_0] = _val_0
		end
	end
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	setmetatable(_base_0, _parent_0.__base)
	_class_0 = setmetatable({
		__init = function(self, ...)
			return _class_0.__parent.__init(self, ...)
		end,
		__base = _base_0,
		__name = "喔哈",
		__parent = _parent_0
	}, {
		__index = function(cls, name)
			local val = rawget(_base_0, name)
			if val == nil then
				local parent = rawget(cls, "__parent")
				if parent then
					return parent[name]
				end
			else
				return val
			end
		end,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	local self = _class_0;
	self["底部"] = function()
		_class_0.__parent["底部"](self)
		_ = _class_0.__parent["你好"]
		local _call_0 = _class_0.__parent
		_call_0["你好"](_call_0, self)
		local _base_1 = _class_0.__parent
		local _fn_0 = _base_1["你好"]
		return _fn_0 and function(...)
			return _fn_0(_base_1, ...)
		end
	end
	self["空间"] = _u9177({
		function()
			_class_0.__parent["空间"](self)
			_ = _class_0.__parent["你好"]
			local _call_0 = _class_0.__parent
			_call_0["你好"](_call_0, self)
			local _base_1 = _class_0.__parent
			local _fn_0 = _base_1["你好"]
			return _fn_0 and function(...)
				return _fn_0(_base_1, ...)
			end
		end
	})
	if _parent_0.__inherited then
		_parent_0.__inherited(_parent_0, _class_0)
	end
	_u5594_u54c8 = _class_0
end
do
	local _u6d4b_u8bd5
	do
		local _class_0
		local _base_0 = {
			["测试"] = function(self)
				return self.__class["如果"] and self.__class["做"](self.__class)
			end
		}
		if _base_0.__index == nil then
			_base_0.__index = _base_0
		end
		_class_0 = setmetatable({
			__init = function(self)
				self.__class["如果"] = true
			end,
			__base = _base_0,
			__name = "测试"
		}, {
			__index = _base_0,
			__call = function(cls, ...)
				local _self_0 = setmetatable({ }, _base_0)
				cls.__init(_self_0, ...)
				return _self_0
			end
		})
		_base_0.__class = _class_0
		local self = _class_0;
		self["做"] = function(self)
			return 1
		end
		_u6d4b_u8bd5 = _class_0
	end
	local _u6d4b_u8bd5_u5b9e_u4f8b = _u6d4b_u8bd5()
	_u6d4b_u8bd5_u5b9e_u4f8b["测试"](_u6d4b_u8bd5_u5b9e_u4f8b)
end
do
	local _u6d4b_u8bd5
	do
		local _class_0
		local _base_0 = {
			["做"] = function(self)
				return 1
			end,
			["测试"] = function(self)
				return self["如果"] and self["做"](self)
			end
		}
		if _base_0.__index == nil then
			_base_0.__index = _base_0
		end
		_class_0 = setmetatable({
			__init = function(self)
				self["如果"] = true
			end,
			__base = _base_0,
			__name = "测试"
		}, {
			__index = _base_0,
			__call = function(cls, ...)
				local _self_0 = setmetatable({ }, _base_0)
				cls.__init(_self_0, ...)
				return _self_0
			end
		})
		_base_0.__class = _class_0
		_u6d4b_u8bd5 = _class_0
	end
	local _u6d4b_u8bd5_u5b9e_u4f8b = _u6d4b_u8bd5()
	_u6d4b_u8bd5_u5b9e_u4f8b["测试"](_u6d4b_u8bd5_u5b9e_u4f8b)
end
do
	local _class_0
	local _parent_0 = _u9752["应用"]
	local _base_0 = {
		["/"] = function(self)
			return {
				json = {
					["状态"] = true
				}
			}
		end
	}
	for _key_0, _val_0 in pairs(_parent_0.__base) do
		if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
			_base_0[_key_0] = _val_0
		end
	end
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	setmetatable(_base_0, _parent_0.__base)
	_class_0 = setmetatable({
		__init = function(self, ...)
			return _class_0.__parent.__init(self, ...)
		end,
		__base = _base_0,
		__parent = _parent_0
	}, {
		__index = function(cls, name)
			local val = rawget(_base_0, name)
			if val == nil then
				local parent = rawget(cls, "__parent")
				if parent then
					return parent[name]
				end
			else
				return val
			end
		end,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	if _parent_0.__inherited then
		_parent_0.__inherited(_parent_0, _class_0)
	end
end
local _u7c7bA
do
	local _class_0
	local _base_0 = { }
	local _list_0 = {
		_u7c7bB,
		_u7c7bC,
		_u7c7bD,
		{
			["值"] = 123
		}
	}
	for _index_0 = 1, #_list_0 do
		local _item_0 = _list_0[_index_0]
		local _cls_0, _mixin_0 = (_item_0.__base ~= nil), _item_0.__base or _item_0
		for _key_0, _val_0 in pairs(_mixin_0) do
			if _base_0[_key_0] == nil and (not _cls_0 or not _key_0:match("^__")) then
				_base_0[_key_0] = _val_0
			end
		end
	end
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "类A"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	_u7c7bA = _class_0
end
local _u793a_u4f8b
do
	local _class_0
	local _base_0 = { }
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "示例"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	local self = _class_0;
	self["字段1"] = 1
	self["字段2"] = self["字段1"] + 1
	_u793a_u4f8b = _class_0
end
local _u968f_u4fbf
do
	local _class_0
	local _base_0 = {
		__mul = function(self, _u53c2_u6570y)
			return self["x字段"] * _u53c2_u6570y
		end,
		["任意名称"] = 123,
		["相加"] = _u76f8_u52a0,
		__add = add
	}
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function(self, _u53c2_u6570)
			self["x字段"] = _u53c2_u6570
		end,
		__base = _base_0,
		__name = "随便"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	_u968f_u4fbf = _class_0
end
return nil
