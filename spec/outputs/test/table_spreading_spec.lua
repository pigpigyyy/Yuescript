return describe("table spreading", function()
	return it("list and dict", function()
		local template = {
			foo = "Hello",
			bar = "World",
			"!"
		}
		local specialized
		do
			local _tab_0 = {
				"a",
				"b",
				"c"
			}
			local _idx_0 = 1
			for _key_0, _value_0 in pairs(template) do
				if _idx_0 == _key_0 then
					_tab_0[#_tab_0 + 1] = _value_0
					_idx_0 = _idx_0 + 1
				else
					_tab_0[_key_0] = _value_0
				end
			end
			_tab_0.bar = "Bob"
			specialized = _tab_0
		end
		return assert.same(specialized, {
			"a",
			"b",
			"c",
			"!",
			foo = "Hello",
			bar = "Bob"
		})
	end)
end)
