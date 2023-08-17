local _u4f60_u597d
do
	local _accum_0 = { }
	local _len_0 = 1
	for _, _u53d8_u91cfx in ipairs({
		1,
		2,
		3,
		4
	}) do
		_accum_0[_len_0] = _u53d8_u91cfx * 2
		_len_0 = _len_0 + 1
	end
	_u4f60_u597d = _accum_0
end
local _u7269_u54c1 = {
	1,
	2,
	3,
	4,
	5,
	6
}
local _
do
	local _accum_0 = { }
	local _len_0 = 1
	for _u53d8_u91cfz in ipairs(_u7269_u54c1) do
		if _u53d8_u91cfz > 4 then
			_accum_0[_len_0] = _u53d8_u91cfz
			_len_0 = _len_0 + 1
		end
	end
	_ = _accum_0
end
local _u534a_u5f84
do
	local _accum_0 = { }
	local _len_0 = 1
	for _u53d8_u91cfa in ipairs({
		1,
		2,
		3,
		4,
		5,
		6
	}) do
		if _u597d_u6570_u5b57(_u53d8_u91cfa) then
			_accum_0[_len_0] = {
				_u53d8_u91cfa
			}
			_len_0 = _len_0 + 1
		end
	end
	_u534a_u5f84 = _accum_0
end
do
	local _accum_0 = { }
	local _len_0 = 1
	for _u53d8_u91cfz in _u7269_u54c1 do
		for _u53d8_u91cfj in _u5217_u8868 do
			if _u53d8_u91cfz > 4 then
				_accum_0[_len_0] = _u53d8_u91cfz
				_len_0 = _len_0 + 1
			end
		end
	end
	_ = _accum_0
end
require("实用")
local _u503e_u5012
_u503e_u5012 = function(_u53d8_u91cfx)
	return _u6253_u5370(_u5b9e_u7528["倾倒"](_u53d8_u91cfx))
end
local _u8303_u56f4
_u8303_u56f4 = function(_u6570_u91cf)
	local _u53d8_u91cfi = 0
	return coroutine.wrap(function()
		while _u53d8_u91cfi < _u6570_u91cf do
			coroutine.yield(_u53d8_u91cfi)
			_u53d8_u91cfi = _u53d8_u91cfi + 1
		end
	end)
end
_u503e_u5012((function()
	local _accum_0 = { }
	local _len_0 = 1
	for _u53d8_u91cfx in _u8303_u56f4(10) do
		_accum_0[_len_0] = _u53d8_u91cfx
		_len_0 = _len_0 + 1
	end
	return _accum_0
end)())
_u503e_u5012((function()
	local _accum_0 = { }
	local _len_0 = 1
	for _u53d8_u91cfx in _u8303_u56f4(5) do
		if _u53d8_u91cfx > 2 then
			for _u53d8_u91cfy in _u8303_u56f4(5) do
				_accum_0[_len_0] = {
					_u53d8_u91cfx,
					_u53d8_u91cfy
				}
				_len_0 = _len_0 + 1
			end
		end
	end
	return _accum_0
end)())
local _u4e1c_u897f
do
	local _accum_0 = { }
	local _len_0 = 1
	for _u53d8_u91cfx in _u8303_u56f4(10) do
		if _u53d8_u91cfx > 5 then
			for _u53d8_u91cfy in _u8303_u56f4(10) do
				if _u53d8_u91cfy > 7 then
					_accum_0[_len_0] = _u53d8_u91cfx + _u53d8_u91cfy
					_len_0 = _len_0 + 1
				end
			end
		end
	end
	_u4e1c_u897f = _accum_0
end
for _u53d8_u91cfx in ipairs({
	1,
	2,
	4
}) do
	for _u53d8_u91cfy in ipairs({
		1,
		2,
		3
	}) do
		if _u53d8_u91cfx ~= 2 then
			_u6253_u5370(_u53d8_u91cfx, _u53d8_u91cfy)
		end
	end
end
for _u53d8_u91cfx in _u7269_u54c1 do
	_u6253_u5370("你好", _u53d8_u91cfx)
end
do
	local _accum_0 = { }
	local _len_0 = 1
	for _u53d8_u91cfx in _u53d8_u91cfx do
		_accum_0[_len_0] = _u53d8_u91cfx
		_len_0 = _len_0 + 1
	end
	_ = _accum_0
end
local _u53d8_u91cfx
do
	local _accum_0 = { }
	local _len_0 = 1
	for _u53d8_u91cfx in _u53d8_u91cfx do
		_accum_0[_len_0] = _u53d8_u91cfx
		_len_0 = _len_0 + 1
	end
	_u53d8_u91cfx = _accum_0
end
for _u53d8_u91cfx in ipairs({
	1,
	2,
	4
}) do
	for _u53d8_u91cfy in ipairs({
		1,
		2,
		3
	}) do
		if _u53d8_u91cfx ~= 2 then
			_u6253_u5370(_u53d8_u91cfx, _u53d8_u91cfy)
		end
	end
end
local _u53cc_u500d
do
	local _accum_0 = { }
	local _len_0 = 1
	for _index_0 = 1, #_u7269_u54c1 do
		local _u53d8_u91cfx = _u7269_u54c1[_index_0]
		_accum_0[_len_0] = _u53d8_u91cfx * 2
		_len_0 = _len_0 + 1
	end
	_u53cc_u500d = _accum_0
end
for _index_0 = 1, #_u53cc_u500d do
	local _u53d8_u91cfx = _u53cc_u500d[_index_0]
	_u6253_u5370(_u53d8_u91cfx)
end
local _u5207
do
	local _accum_0 = { }
	local _len_0 = 1
	for _index_0 = 1, #_u7269_u54c1 do
		local _u53d8_u91cfx = _u7269_u54c1[_index_0]
		if _u53d8_u91cfx > 3 then
			_accum_0[_len_0] = _u53d8_u91cfx
			_len_0 = _len_0 + 1
		end
	end
	_u5207 = _accum_0
end
do
	local _accum_0 = { }
	local _len_0 = 1
	for _index_0 = 1, #_u7269_u54c1 do
		local _u53d8_u91cfx = _u7269_u54c1[_index_0]
		for _index_1 = 1, #_u7269_u54c1 do
			local _u53d8_u91cfy = _u7269_u54c1[_index_1]
			_accum_0[_len_0] = _u53d8_u91cfx + _u53d8_u91cfy
			_len_0 = _len_0 + 1
		end
	end
	_u4f60_u597d = _accum_0
end
for _index_0 = 1, #_u4f60_u597d do
	local _u53d8_u91cfz = _u4f60_u597d[_index_0]
	_u6253_u5370(_u53d8_u91cfz)
end
_u53d8_u91cfx = {
	1,
	2,
	3,
	4,
	5,
	6,
	7
}
local _max_0 = -5
for _index_0 = 2, _max_0 < 0 and #_u53d8_u91cfx + _max_0 or _max_0, 2 do
	local _u53d8_u91cfy = _u53d8_u91cfx[_index_0]
	_u6253_u5370(_u53d8_u91cfy)
end
local _max_1 = 3
for _index_0 = 1, _max_1 < 0 and #_u53d8_u91cfx + _max_1 or _max_1 do
	local _u53d8_u91cfy = _u53d8_u91cfx[_index_0]
	_u6253_u5370(_u53d8_u91cfy)
end
for _index_0 = 2, #_u53d8_u91cfx do
	local _u53d8_u91cfy = _u53d8_u91cfx[_index_0]
	_u6253_u5370(_u53d8_u91cfy)
end
for _index_0 = 1, #_u53d8_u91cfx, 2 do
	local _u53d8_u91cfy = _u53d8_u91cfx[_index_0]
	_u6253_u5370(_u53d8_u91cfy)
end
for _index_0 = 2, #_u53d8_u91cfx, 2 do
	local _u53d8_u91cfy = _u53d8_u91cfx[_index_0]
	_u6253_u5370(_u53d8_u91cfy)
end
local _u53d8_u91cfa, _u53d8_u91cfb, _u53d8_u91cfc = 1, 5, 2
local _max_2 = _u53d8_u91cfb
for _index_0 = _u53d8_u91cfa, _max_2 < 0 and #_u53d8_u91cfx + _max_2 or _max_2, _u53d8_u91cfc do
	local _u53d8_u91cfy = _u53d8_u91cfx[_index_0]
	_u6253_u5370(_u53d8_u91cfy)
end
local _u6b63_u5e38
_u6b63_u5e38 = function(_u4f60_u597d)
	local _accum_0 = { }
	local _len_0 = 1
	for _u53d8_u91cfx in _u55ef_u54fc do
		_accum_0[_len_0] = _u53d8_u91cfx
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
local _u6d4b_u8bd5 = _u53d8_u91cfx(1, 2, 3, 4, 5)
for _index_0 = 1, #_u6d4b_u8bd5 do
	local _u4e8b_u60c5 = _u6d4b_u8bd5[_index_0]
	_u6253_u5370(_u4e8b_u60c5)
end
return function()
	local _list_0 = _u884c_u4eec
	for _index_0 = 1, #_list_0 do
		local _u884c = _list_0[_index_0]
		_u53d8_u91cfa = _u53d8_u91cfb
	end
end
