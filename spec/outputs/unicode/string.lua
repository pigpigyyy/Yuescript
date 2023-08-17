local _u4f60_u597d = "你好"
local _u95ee_u5019 = "啥事，好吧"
_u6253_u5370(_u4f60_u597d)
local _u55ef = '嗯'
local _u8fd9_u91cc, _u53e6_u4e00_u4e2a = "好的", '世界'
local _u5440 = "YU'M"
_u4f60('我说："嗯"')
_u6253_u5370(_u5440, _u4f60)
_u53e6_u4e00_u4e2a = [[ 你好 世界 ]]
local _u4f60_u597d__u90a3_u91cc = [[  你好呀
]]
local _u597d_u5427 = [==[ "helo" ]==]
local _u54c8_u55bd = [===[  eat noots]===]
local _u55ef_u55ef = [[well trhere]]
local _u6587_u5b57 = [[
nil
Fail to compile
]]
_u6587_u5b57[ [[abc]]] = [["#{变量i}" for 变量i = 1, 10] for 变量i = 1, 10]]
local _u54e6_u54e6 = ""
local _u53d8_u91cfx = "\\"
_u53d8_u91cfx = "a\\b"
_u53d8_u91cfx = "\\\n"
_u53d8_u91cfx = "\""
local _u53d8_u91cfa = "你好 " .. tostring(_u95ee_u5019) .. " 你好"
local _u53d8_u91cfb = tostring(_u95ee_u5019) .. " 你好"
local _u53d8_u91cfc = "你好 " .. tostring(5 + 1)
local _u53d8_u91cfd = tostring(_u4f60_u597d(_u4e16_u754c))
local _u53d8_u91cfe = tostring(1) .. " " .. tostring(2) .. " " .. tostring(3)
local _u53d8_u91cff = [[你好 #{世界} 世界]]
local _u53d8_u91cfg = "#{你好 世界}"
_u53d8_u91cfa = '你好 #{问候} 你好'
_u53d8_u91cfb = '#{问候} 你好'
_u53d8_u91cfc = '你好 #{问候}'
local _u53d8_u91cf_ = "你好"
local _call_0 = ("你好")
_call_0["格式"](_call_0, 1)
local _call_1 = ("你好")
_call_1["格式"](_call_1, 1, 2, 3)
local _call_2 = ("你好")
_call_2["格式"](_call_2, 1, 2, 3)(1, 2, 3)
local _call_3 = ("你好")
_call_3["世界"](_call_3)
local _call_4 = ("你好")
_call_4["格式"](_call_4)["问候"](1, 2, 3)
local _call_5 = ("你好")
_call_5["格式"](_call_5, 1, 2, 3)
local _call_6 = _u67d0_u4e8b("你好")
_call_6["世界"](_call_6)
return _u67d0_u4e8b((function()
	local _call_7 = ("你好")
	return _call_7["世界"](_call_7)
end)())
