do
	local _u53d8_u91cfa = 0
	::_u5f00_u59cb::
	_u53d8_u91cfa = _u53d8_u91cfa + 1
	if _u53d8_u91cfa == 5 then
		goto _u7ed3_u675f
	end
	goto _u5f00_u59cb
	::_u7ed3_u675f::
end
do
	for _u8ba1_u6570z = 1, 10 do
		for _u8ba1_u6570y = 1, 10 do
			for _u8ba1_u6570x = 1, 10 do
				if _u8ba1_u6570x ^ 2 + _u8ba1_u6570y ^ 2 == _u8ba1_u6570z ^ 2 then
					_u6253_u5370('找到了毕达哥拉斯三元组:', _u8ba1_u6570x, _u8ba1_u6570y, _u8ba1_u6570z)
					goto _u5b8c_u6210
				end
			end
		end
	end
	::_u5b8c_u6210::
end
do
	for _u8ba1_u6570z = 1, 10 do
		for _u8ba1_u6570y = 1, 10 do
			for _u8ba1_u6570x = 1, 10 do
				if _u8ba1_u6570x ^ 2 + _u8ba1_u6570y ^ 2 == _u8ba1_u6570z ^ 2 then
					_u6253_u5370('找到了毕达哥拉斯三元组:', _u8ba1_u6570x, _u8ba1_u6570y, _u8ba1_u6570z)
					_u6253_u5370('now trying next z...')
					goto _u7ee7_u7eedz
				end
			end
		end
		::_u7ee7_u7eedz::
	end
end
do
	::_u91cd_u505a::
	for _u8ba1_u6570x = 1, 10 do
		for _u8ba1_u6570y = 1, 10 do
			if not _u51fd_u6570f(_u8ba1_u6570x, _u8ba1_u6570y) then
				goto _u7ee7_u7eed
			end
			if not _u51fd_u6570g(_u8ba1_u6570x, _u8ba1_u6570y) then
				goto _u8df3_u8fc7
			end
			if not _u51fd_u6570h(_u8ba1_u6570x, _u8ba1_u6570y) then
				goto _u91cd_u505a
			end
			::_u7ee7_u7eed::
		end
	end
	::_u8df3_u8fc7::
end
do
	local _list_0 = _u5217_u8868
	for _index_0 = 1, #_list_0 do
		local _u53d8_u91cf = _list_0[_index_0]
		if _u53d8_u91cf % 2 == 0 then
			_u6253_u5370('列表有偶数')
			goto _u6709
		end
	end
	_u6253_u5370('列表没偶数')
	::_u6709::
end
