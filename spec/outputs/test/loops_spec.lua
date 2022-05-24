return describe("loops", function()
	return it("should continue", function()
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
		return assert.same(output, {
			2,
			4,
			6
		})
	end)
end)
