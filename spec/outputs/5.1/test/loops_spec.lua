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
				local _continue_0 = false
				repeat
					if x % 2 == 1 then
						_continue_0 = true
						break
					end
					_accum_0[_len_0] = x
					_len_0 = _len_0 + 1
					_continue_0 = true
				until true
				if not _continue_0 then
					break
				end
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
			local _cond_0 = false
			local _continue_0 = false
			repeat
				a = a + 1
				if a == 3 then
					_cond_0 = a == 8
					_continue_0 = true
					break
				end
				if a == 5 then
					break
				end
				output[#output + 1] = a
				_cond_0 = a == 8
				_continue_0 = true
			until true
			if not _continue_0 then
				break
			end
		until _cond_0
		return assert.same({
			1,
			2,
			4
		}, output)
	end)
end)
