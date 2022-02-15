local x = a.b.c.d
x.y = a.b:c()
func(1, arg2.value:get(), arg3.value:get(), {
	1,
	((function()
		if x ~= nil then
			local _obj_0 = x.y
			if _obj_0 ~= nil then
				return _obj_0.z
			end
			return nil
		end
		return nil
	end)() ~= nil),
	3
})
local tbb = {
	k1 = a:b(123).c(),
	k2 = {
		w1 = a().b:c()
	}
}
local tb
do
	local _obj_0 = f2
	if _obj_0 ~= nil then
		tb = _obj_0("abc", f1({ }).a)
	end
end
local f
f = function()
	local _accum_0 = { }
	local _len_0 = 1
	local _list_0 = vals
	for _index_0 = 1, #_list_0 do
		local _des_0 = _list_0[_index_0]
		local a = _des_0[1]
		_accum_0[_len_0] = a.b:c(123)
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
local f1
f1 = function()
	return x, a:b(123).c("abc")
end
local result = origin.transform.root.gameObject:Parents():Descendants():SelectEnable():SelectVisible():TagEqual("fx"):Where(function(x)
	if x:IsTargeted() then
		return false
	end
	return x.name:EndsWith("(Clone)")
end):Destroy()
origin.transform.root.gameObject:Parents():Descendants():SelectEnable():SelectVisible():TagEqual("fx"):Where(function(x)
	return x.name:EndsWith("(Clone)")
end):Destroy()
do
	local _with_0 = item
	_with_0.itemFieldA = 123
	_with_0:callMethod():chainCall()
	_with_0:callMethod():chainCall()
	_with_0:chainCall()
	do
		local _exp_0 = _with_0.itemFieldB:getValue()
		if "Valid" == _exp_0 or _with_0:getItemState() == _exp_0 then
			_with_0:itemMethodA():getValue()
		else
			_with_0:itemMethodB():getValue()
		end
	end
	local a
	if _with_0.itemFieldC then
		a = _with_0.itemFieldD
	else
		a = _with_0.itemFieldE
	end
	local _list_0 = values
	for _index_0 = 1, #_list_0 do
		local v = _list_0[_index_0]
		_with_0:itemMethodC(v)
	end
	for i = 1, counter do
		_with_0:itemMethodC(i)
	end
	if not _with_0.b then
		do
			local _accum_0 = { }
			local _len_0 = 1
			while _with_0.itemFieldD do
				_accum_0[_len_0] = _with_0:itemNext():get()
				_len_0 = _len_0 + 1
			end
			_with_0.c = _accum_0
		end
	end
end
return nil
