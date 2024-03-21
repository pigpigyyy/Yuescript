do
	local _u53d8_u91cfa
	local _exp_0 = _u53d8_u91cfb
	if _exp_0 ~= nil then
		_u53d8_u91cfa = _exp_0
	else
		_u53d8_u91cfa = _u53d8_u91cfc
	end
end
do
	local _u5b57_u6bb5a
	local _exp_0 = _u53d8_u91cfb
	if _exp_0 ~= nil then
		_u5b57_u6bb5a = _exp_0["字段a"]
	else
		_u5b57_u6bb5a = _u53d8_u91cfc["字段a"]
	end
end
do
	local _u53d8_u91cfa = _u51fd_u6570()
	if _u53d8_u91cfa == nil then
		_u53d8_u91cfa = false
	end
end
do
	local _u53d8_u91cfa
	if _u53d8_u91cfa == nil then
		_u53d8_u91cfa = _u51fd_u6570()
	end
end
do
	local _u53d8_u91cfa, _u53d8_u91cfb, _u53d8_u91cfc, _u53d8_u91cfd, _u53d8_u91cfe
	if _u53d8_u91cfb ~= nil then
		_u53d8_u91cfa = _u53d8_u91cfb
	else
		if _u53d8_u91cfc ~= nil then
			_u53d8_u91cfa = _u53d8_u91cfc
		else
			if _u53d8_u91cfd ~= nil then
				_u53d8_u91cfa = _u53d8_u91cfd
			else
				_u53d8_u91cfa = _u53d8_u91cfe
			end
		end
	end
end
do
	local _u53d8_u91cfa
	local _exp_0 = _u53d8_u91cfb
	if _exp_0 ~= nil then
		_u53d8_u91cfa = _exp_0
	else
		do
			local _exp_1 = _u53d8_u91cfc
			if _exp_1 ~= nil then
				_u53d8_u91cfa = _exp_1
			else
				do
					local _exp_2 = _u53d8_u91cfd
					if _exp_2 ~= nil then
						_u53d8_u91cfa = _exp_2
					else
						_u53d8_u91cfa = _u53d8_u91cfe
					end
				end
			end
		end
	end
end
do
	_u51fd_u6570((function()
		local _exp_0 = _u53d8_u91cfx
		if _exp_0 ~= nil then
			return _exp_0
		else
			return "你好"
		end
	end)())
end
do
	local _with_0
	local _exp_0 = _u51fd_u6570A()
	if _exp_0 ~= nil then
		_with_0 = _exp_0
	else
		_with_0 = _u51fd_u6570B()
	end
	_u6253_u5370(_with_0["字段"])
end
do
	local _u53d8_u91cfa = 1 + 2 + (function()
		local _exp_0 = _u53d8_u91cfb
		if _exp_0 ~= nil then
			return _exp_0
		else
			return _u53d8_u91cfc + 3 + 4
		end
	end)()
end
do
	local _u53d8_u91cfa = 1 + 2 + ((function()
		local _exp_0 = _u53d8_u91cfb
		if _exp_0 ~= nil then
			return _exp_0
		else
			return _u53d8_u91cfc
		end
	end)()) + 3 + 4
end
do
	local _u5b57_u6bb5a, _u5b57_u6bb5b
	do
		local _obj_0 = _u51fd_u6570()
		_u5b57_u6bb5a, _u5b57_u6bb5b = _obj_0["字段a"], _obj_0["字段b"]
	end
	local _u53d8_u91cfa = _u53d8_u91cfa or 1
	local _u53d8_u91cfb
	if _u53d8_u91cfb == nil then
		do
			local _exp_0 = _u53d8_u91cfc
			if _exp_0 ~= nil then
				_u53d8_u91cfb = _exp_0
			else
				_u53d8_u91cfb = 2
			end
		end
	end
end
do
	local _u53d8_u91cfa
	local _exp_0 = 1
	if _exp_0 ~= nil then
		_u53d8_u91cfa = _exp_0
	else
		do
			local _exp_1 = 2
			if _exp_1 ~= nil then
				_u53d8_u91cfa = _exp_1
			else
				_u53d8_u91cfa = 3
			end
		end
	end
end
do
	local _obj_0 = _u53d8_u91cfa["字段"]
	if _obj_0["字段"] == nil then
		_obj_0["字段"] = 1
	end
	local _u53d8_u91cfa
	do
		local _exp_0 = _u53d8_u91cfb["字段"]
		if _exp_0 ~= nil then
			_u53d8_u91cfa = _exp_0
		else
			do
				local _exp_1 = _u53d8_u91cfc["字段"]
				if _exp_1 ~= nil then
					_u53d8_u91cfa = _exp_1
				else
					_u53d8_u91cfa = _u53d8_u91cfd["字段"]
				end
			end
		end
	end
	if _u53d8_u91cfa["字段"] == nil then
		_u53d8_u91cfa["字段"] = 1
	end
end
return nil
