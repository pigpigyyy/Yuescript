local _u5e38_u91cfa <const>, _u5e38_u91cfb <const>, _u5e38_u91cfc <const>, _u5e38_u91cfd <const> = 1, 2, 3, 4
do
	local _u5173_u95ed_u53d8_u91cfa, _u5173_u95ed_u53d8_u91cfb = setmetatable({ }, {
		__close = function(self)
			return _u6253_u5370("已关闭")
		end
	})
	local _close_0 <close> = _u5173_u95ed_u53d8_u91cfa
	local _close_1 <close> = _u5173_u95ed_u53d8_u91cfb
	local _u53d8_u91cfc <const>, _u5e38_u91cfd <const> = 123, 'abc'
	close(_u5e38_u91cfa, _u5e38_u91cfb)
	const(_u5e38_u91cfc, _u5e38_u91cfd)
end
do
	local _u5e38_u91cfa <const> = _u51fd_u6570()
	local _u5143_u7d20b, _u5143_u7d20c, _u5143_u7d20d
	do
		local _obj_0, _obj_1 = _u51fd_u65701()
		_u5143_u7d20b, _u5143_u7d20c = _obj_0[1], _obj_0[2]
		_u5143_u7d20d = _obj_1[1]
	end
end
do
	local _u5e38_u91cfa, _u5143_u7d20b, _u5143_u7d20c, _u5143_u7d20d
	do
		local _obj_0, _obj_1, _obj_2 = _u51fd_u6570()
		_u5e38_u91cfa = _obj_0
		_u5143_u7d20b, _u5143_u7d20c = _obj_1[1], _obj_1[2]
		_u5143_u7d20d = _obj_2[1]
	end
end
do
	local _u5173_u95ed_u53d8_u91cfv <close> = (function()
		if _u6807_u8bb0_u4e3a_u771f then
			return _u51fd_u6570_u8c03_u7528()
		else
			return setmetatable({ }, {
				__close = function(self) end
			})
		end
	end)()
	local _u5173_u95ed_u53d8_u91cff <close> = (function()
		local _with_0 = io.open("文件.txt")
		_with_0:write("你好")
		return _with_0
	end)()
end
do
	local _u5e38_u91cfa <const> = (function()
		if true then
			return 1
		end
	end)()
	local _u5173_u95ed_u53d8_u91cfb <close> = (function()
		if not false then
			if _u6761_u4ef6x then
				return 1
			end
		end
	end)()
	local _u5e38_u91cfc <const> = (function()
		if true then
			local _exp_0 = _u6761_u4ef6x
			if "abc" == _exp_0 then
				return 998
			end
		end
	end)()
	local _u5173_u95ed_u53d8_u91cfd <close> = (function()
		if (function()
			local _exp_0 = _u6761_u4ef6a
			if _exp_0 ~= nil then
				return _exp_0
			else
				return _u6761_u4ef6b
			end
		end)() then
			return {
				["数值"] = _u6570_u503c
			}
		end
	end)()
end
do
	local __u65e0_u6548_u53d8_u91cf <close> = (function()
		local _with_0 = io.open("文件.txt")
		_with_0:write("你好")
		return _with_0
	end)()
	local __u65e0_u6548_u53d8_u91cf <close> = setmetatable({ }, {
		__close = function()
			return _u6253_u5370("第二")
		end
	})
	local __u65e0_u6548_u53d8_u91cf <close> = setmetatable({ }, {
		__close = function()
			return _u6253_u5370("第一")
		end
	})
end
local __u5ef6_u8fdf_u5bf9_u8c61_u6570_u7ec4 = setmetatable({ }, {
	__close = function(self)
		self[#self]()
		self[#self] = nil
	end
})
local _u5ef6_u8fdf
_u5ef6_u8fdf = function(_u9879_u76ee)
	__u5ef6_u8fdf_u5bf9_u8c61_u6570_u7ec4[#__u5ef6_u8fdf_u5bf9_u8c61_u6570_u7ec4 + 1] = _u9879_u76ee
	return __u5ef6_u8fdf_u5bf9_u8c61_u6570_u7ec4
end
do
	local __u65e0_u6548_u53d8_u91cf <close> = _u5ef6_u8fdf(function()
		return _u6253_u5370(3)
	end)
	local __u65e0_u6548_u53d8_u91cf <close> = _u5ef6_u8fdf(function()
		return _u6253_u5370(2)
	end)
	local __u65e0_u6548_u53d8_u91cf <close> = _u5ef6_u8fdf(function()
		return _u6253_u5370(1)
	end)
end
