return describe("loops", function()
	it("should continue", function()
		local input = {
			1,
			2,
			3,
			4,
			5,
			6
		}
		local output
		do
			local _accum_0 = { }
			local _len_0 = 1
			for _index_0 = 1, #input do
				local x = input[_index_0]
				if x % 2 == 1 then
					goto _continue_0
				end
				_accum_0[_len_0] = x
				_len_0 = _len_0 + 1
				::_continue_0::
			end
			output = _accum_0
		end
		return assert.same({
			2,
			4,
			6
		}, output)
	end)
	return it("continue in repeat", function()
		local output = { }
		local a = 0
		repeat
			a = a + 1
			if a == 3 then
				goto _continue_0
			end
			if a == 5 then
				break
			end
			output[#output + 1] = a
			::_continue_0::
		until a == 8
		return assert.same({
			1,
			2,
			4
		}, output)
	end)
end)
