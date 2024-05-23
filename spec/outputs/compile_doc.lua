local outputFolder = ...
local _list_0 = {
	{
		"codes_from_doc.lua",
		"doc/docs/doc/README.md"
	},
	{
		"codes_from_doc_zh.lua",
		"doc/docs/zh/doc/README.md"
	}
}
for _index_0 = 1, #_list_0 do
	local _des_0 = _list_0[_index_0]
	local compiledFile, docFile = _des_0[1], _des_0[2]
	local input
	local _with_0 = io.open(docFile)
	if _with_0 ~= nil then
		local to_lua = require("yue").to_lua
		local text = _with_0:read("*a")
		local codes = { }
		for code in text:gmatch("```moonscript(.-)```") do
			local result, err = to_lua(code, {
				implicit_return_root = false,
				reserve_line_number = false
			})
			if result then
				codes[#codes + 1] = result
			elseif not err:match("macro exporting module only accepts macro definition") then
				print(err)
				os.exit(1)
			end
		end
		for code in text:gmatch("<pre>(.-)</pre>") do
			local result, err = to_lua(code:gsub("&lt;", "<"):gsub("&gt;", ">"), {
				implicit_return_root = false,
				reserve_line_number = false
			})
			if result then
				codes[#codes + 1] = result
			else
				print(err)
				os.exit(1)
			end
		end
		local output
		local _with_1 = io.open(tostring(outputFolder) .. "/" .. tostring(compiledFile), "w+")
		_with_1:write(table.concat(codes))
		output = _with_1
		local _close_0 <close> = output
	end
	input = _with_0
	local _close_0 <close> = input
end
