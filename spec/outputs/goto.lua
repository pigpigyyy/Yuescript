do
	local a = 0
	::start::
	a = a + 1
	if a == 5 then
		goto done
	end
	goto start
	::done::
end
do
	for z = 1, 10 do
		for y = 1, 10 do
			for x = 1, 10 do
				if x ^ 2 + y ^ 2 == z ^ 2 then
					print('found a Pythagorean triple:', x, y, z)
					goto done
				end
			end
		end
	end
	::done::
end
do
	for z = 1, 10 do
		for y = 1, 10 do
			for x = 1, 10 do
				if x ^ 2 + y ^ 2 == z ^ 2 then
					print('found a Pythagorean triple:', x, y, z)
					print('now trying next z...')
					goto zcontinue
				end
			end
		end
		::zcontinue::
	end
end
do
	::redo::
	for x = 1, 10 do
		for y = 1, 10 do
			if not f(x, y) then
				goto continue
			end
			if not g(x, y) then
				goto skip
			end
			if not h(x, y) then
				goto redo
			end
			::continue::
		end
	end
	::skip::
end
do
	local _list_0 = t
	for _index_0 = 1, #_list_0 do
		local x = _list_0[_index_0]
		if x % 2 == 0 then
			print('list has even number')
			goto has
		end
	end
	print('list lacks even number')
	::has::
end
