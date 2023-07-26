R"lua_codes(
--[[
The MIT License (MIT)

Copyright (c) 2012-2013 Mark Langen, modified by Li Jin 2023

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.]]

--
-- Util.lua
--
-- Provides some common utilities shared throughout the project.
--

local function lookupify(tb)
	for _, v in pairs(tb) do
		tb[v] = true
	end
	return tb
end


local function CountTable(tb)
	local c = 0
	for _ in pairs(tb) do c = c + 1 end
	return c
end


local function PrintTable(tb, atIndent)
	if tb.Print then
		return tb.Print()
	end
	atIndent = atIndent or 0
	local useNewlines = (CountTable(tb) > 1)
	local baseIndent = string.rep('    ', atIndent+1)
	local out = "{"..(useNewlines and '\n' or '')
	for k, v in pairs(tb) do
		if type(v) ~= 'function' then
		--do
			out = out..(useNewlines and baseIndent or '')
			if type(k) == 'number' then
				--nothing to do
			elseif type(k) == 'string' and k:match("^[A-Za-z_][A-Za-z0-9_]*$") then 
				out = out..k.." = "
			elseif type(k) == 'string' then
				out = out.."[\""..k.."\"] = "
			else
				out = out.."["..tostring(k).."] = "
			end
			if type(v) == 'string' then
				out = out.."\""..v.."\""
			elseif type(v) == 'number' then
				out = out..v
			elseif type(v) == 'table' then
				out = out..PrintTable(v, atIndent+(useNewlines and 1 or 0))
			else
				out = out..tostring(v)
			end
			if next(tb, k) then
				out = out..","
			end
			if useNewlines then
				out = out..'\n'
			end
		end
	end
	out = out..(useNewlines and string.rep('    ', atIndent) or '').."}"
	return out
end


local blacklist = {
	["do"] = true,
	["if"] = true,
	["in"] = true,
	["or"] = true,
	["for"] = true,
	["and"] = true,
	["not"] = true,
	["end"] = true,
	["nil"] = true
}

local insert, char = table.insert, string.char

local chars = {}
for i = 97, 122 do
	insert(chars, char(i))
end
for i = 65, 90 do
	insert(chars, char(i))
end

local function GetUnique(self)
	for x = 1, 52 do
		local c = chars[x]
		if not blacklist[c] and not self:GetVariable(c) then
			return c
		end
	end
	for x = 1, 52 do
		for y = 1, 52 do
			local c = chars[x]..chars[y]
			if not blacklist[c] and not self:GetVariable(c) then
				return c
			end
		end
	end
	for x = 1, 52 do
		for y = 1, 52 do
			for z = 1, 52 do
				local c = chars[x]..chars[y]..chars[z]
				if not blacklist[c] and not self:GetVariable(c) then
					return c
				end
			end
		end
	end
end

local Scope = {
	new = function(self, parent)
		local s = {
			Parent = parent,
			Locals = { },
			Globals = { },
			oldLocalNamesMap = { },
			oldGlobalNamesMap = { },
			Children = { },
		}

		if parent then
			table.insert(parent.Children, s)
		end

		return setmetatable(s, { __index = self })
	end,

	AddLocal = function(self, v)
		table.insert(self.Locals, v)
	end,

	AddGlobal = function(self, v)
		table.insert(self.Globals, v)
	end,

	CreateLocal = function(self, name)
		local v
		v = self:GetLocal(name)
		if v then return v end
		v = { }
		v.Scope = self
		v.Name = name
		v.IsGlobal = false
		v.CanRename = true
		v.References = 1
		self:AddLocal(v)
		return v
	end,

	GetLocal = function(self, name)
		for k, var in pairs(self.Locals) do
			if var.Name == name then return var end
		end

		if self.Parent then
			return self.Parent:GetLocal(name)
		end
	end,

	GetOldLocal = function(self, name)
		if self.oldLocalNamesMap[name] then
			return self.oldLocalNamesMap[name]
		end
		return self:GetLocal(name)
	end,

	mapLocal = function(self, name, var)
		self.oldLocalNamesMap[name] = var
	end,

	GetOldGlobal = function(self, name)
		if self.oldGlobalNamesMap[name] then
			return self.oldGlobalNamesMap[name]
		end
		return self:GetGlobal(name)
	end,

	mapGlobal = function(self, name, var)
		self.oldGlobalNamesMap[name] = var
	end,

	GetOldVariable = function(self, name)
		return self:GetOldLocal(name) or self:GetOldGlobal(name)
	end,

	RenameLocal = function(self, oldName, newName)
		oldName = type(oldName) == 'string' and oldName or oldName.Name
		local found = false
		local var = self:GetLocal(oldName)
		if var then
			var.Name = newName
			self:mapLocal(oldName, var)
			found = true
		end
		if not found and self.Parent then
			self.Parent:RenameLocal(oldName, newName)
		end
	end,

	RenameGlobal = function(self, oldName, newName)
		oldName = type(oldName) == 'string' and oldName or oldName.Name
		local found = false
		local var = self:GetGlobal(oldName)
		if var then
			var.Name = newName
			self:mapGlobal(oldName, var)
			found = true
		end
		if not found and self.Parent then
			self.Parent:RenameGlobal(oldName, newName)
		end
	end,

	RenameVariable = function(self, oldName, newName)
		oldName = type(oldName) == 'string' and oldName or oldName.Name
		if self:GetLocal(oldName) then
			self:RenameLocal(oldName, newName)
		else
			self:RenameGlobal(oldName, newName)
		end
	end,

	GetAllVariables = function(self)
		local ret = self:getVars(true) -- down
		for k, v in pairs(self:getVars(false)) do -- up
			table.insert(ret, v)
		end
		return ret
	end,

	getVars = function(self, top)
		local ret = { }
		if top then
			for k, v in pairs(self.Children) do
				for k2, v2 in pairs(v:getVars(true)) do
					table.insert(ret, v2)
				end
			end
		else
			for k, v in pairs(self.Locals) do
				table.insert(ret, v)
			end
			for k, v in pairs(self.Globals) do
				table.insert(ret, v)
			end
			if self.Parent then
				for k, v in pairs(self.Parent:getVars(false)) do
					table.insert(ret, v)
				end
			end
		end
		return ret
	end,

	CreateGlobal = function(self, name)
		local v
		v = self:GetGlobal(name)
		if v then return v end
		v = { }
		v.Scope = self
		v.Name = name
		v.IsGlobal = true
		v.CanRename = true
		v.References = 1
		self:AddGlobal(v)
		return v
	end,

	GetGlobal = function(self, name)
		for k, v in pairs(self.Globals) do
			if v.Name == name then return v end
		end

		if self.Parent then
			return self.Parent:GetGlobal(name)
		end
	end,

	GetVariable = function(self, name)
		return self:GetLocal(name) or self:GetGlobal(name)
	end,

	ObfuscateLocals = function(self, recommendedMaxLength, validNameChars)
		for i, var in pairs(self.Locals) do
			if var.Name == "_ENV" then
				self:RenameLocal(var.Name, "_ENV")
			else
				local id = GetUnique(self)
				self:RenameLocal(var.Name, id)
			end
		end
	end
}
)lua_codes"

R"lua_codes(
--
-- ParseLua.lua
--
-- The main lua parser and lexer.
-- LexLua returns a Lua token stream, with tokens that preserve
-- all whitespace formatting information.
-- ParseLua returns an AST, internally relying on LexLua.
--

local LowerChars = lookupify{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
							 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
							 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'}
local UpperChars = lookupify{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
							 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
							 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'}
local Digits = lookupify{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'}
local HexDigits = lookupify{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
							'A', 'a', 'B', 'b', 'C', 'c', 'D', 'd', 'E', 'e', 'F', 'f'}

local Symbols = lookupify{'+', '-', '*', '/', '^', '%', ',', '{', '}', '[', ']', '(', ')', ';', '#'}

local Keywords = lookupify{
	'and', 'break', 'do', 'else', 'elseif',
	'end', 'false', 'for', 'function', 'goto', 'if',
	'in', 'local', 'nil', 'not', 'or', 'repeat',
	'return', 'then', 'true', 'until', 'while',
};

local function LexLua(src)
	--token dump
	local tokens = {}

	local st, err = pcall(function()
		--line / char / pointer tracking
		local p = 1
		local line = 1
		local char = 1

		--get / peek functions
		local function get()
			local c = src:sub(p,p)
			if c == '\n' then
				char = 1
				line = line + 1
			else
				char = char + 1
			end
			p = p + 1
			return c
		end
		local function peek(n)
			n = n or 0
			return src:sub(p+n,p+n)
		end
		local function consume(chars)
			local c = peek()
			for i = 1, #chars do
				if c == chars:sub(i,i) then return get() end
			end
		end

		--shared stuff
		local function generateError(err)
			return error(">> :"..line..":"..char..": "..err, 0)
		end

		local function tryGetLongString()
			local start = p
			if peek() == '[' then
				local equalsCount = 0
				local depth = 1
				while peek(equalsCount+1) == '=' do
					equalsCount = equalsCount + 1
				end
				if peek(equalsCount+1) == '[' then
					--start parsing the string. Strip the starting bit
					for _ = 0, equalsCount+1 do get() end

					--get the contents
					local contentStart = p
					while true do
						--check for eof
						if peek() == '' then
							generateError("Expected `]"..string.rep('=', equalsCount).."]` near <eof>.", 3)
						end

						--check for the end
						local foundEnd = true
						if peek() == ']' then
							for i = 1, equalsCount do
								if peek(i) ~= '=' then foundEnd = false end
							end
							if peek(equalsCount+1) ~= ']' then
								foundEnd = false
							end
						else
							if peek() == '[' then
								-- is there an embedded long string?
								local embedded = true
								for i = 1, equalsCount do
									if peek(i) ~= '=' then
										embedded = false
										break
									end
								end
								if peek(equalsCount + 1) == '[' and embedded then
									-- oh look, there was
									depth = depth + 1
									for i = 1, (equalsCount + 2) do
										get()
									end
								end
							end
							foundEnd = false
						end
						--
						if foundEnd then
							depth = depth - 1
							if depth == 0 then
								break
							else
								for i = 1, equalsCount + 2 do
									get()
								end
							end
						else
							get()
						end
					end

					--get the interior string
					local contentString = src:sub(contentStart, p-1)

					--found the end. Get rid of the trailing bit
					for i = 0, equalsCount+1 do get() end

					--get the exterior string
					local longString = src:sub(start, p-1)

					--return the stuff
					return contentString, longString
				else
					return nil
				end
			else
				return nil
			end
		end

		--main token emitting loop
		while true do
			--get leading whitespace. The leading whitespace will include any comments
			--preceding the token. This prevents the parser needing to deal with comments
			--separately.
			local leading = { }
			local leadingWhite = ''
			local longStr = false
			while true do
				local c = peek()
				if c == '#' and peek(1) == '!' and line == 1 then
					-- #! shebang for linux scripts
					get()
					get()
					leadingWhite = "#!"
					while peek() ~= '\n' and peek() ~= '' do
						leadingWhite = leadingWhite .. get()
					end
					local token = {
						Type = 'Comment',
						CommentType = 'Shebang',
						Data = leadingWhite,
						Line = line,
						Char = char
					}
					token.Print = function()
						return "<"..(token.Type .. string.rep(' ', 7-#token.Type)).."  "..(token.Data or '').." >"
					end
					leadingWhite = ""
					table.insert(leading, token)
				end
				if c == ' ' or c == '\t' then
					--whitespace
					--leadingWhite = leadingWhite..get()
					local c2 = get() -- ignore whitespace
					table.insert(leading, { Type = 'Whitespace', Line = line, Char = char, Data = c2 })
				elseif c == '\n' or c == '\r' then
					local nl = get()
					if leadingWhite ~= "" then
						local token = {
							Type = 'Comment',
							CommentType = longStr and 'LongComment' or 'Comment',
							Data = leadingWhite,
							Line = line,
							Char = char,
						}
						token.Print = function()
							return "<"..(token.Type .. string.rep(' ', 7-#token.Type)).."  "..(token.Data or '').." >"
						end
						table.insert(leading, token)
						leadingWhite = ""
					end
					table.insert(leading, { Type = 'Whitespace', Line = line, Char = char, Data = nl })
				elseif c == '-' and peek(1) == '-' then
					--comment
					get()
					get()
					leadingWhite = leadingWhite .. '--'
					local _, wholeText = tryGetLongString()
					if wholeText then
						leadingWhite = leadingWhite..wholeText
						longStr = true
					else
						while peek() ~= '\n' and peek() ~= '' do
							leadingWhite = leadingWhite..get()
						end
					end
				else
					break
				end
			end
			if leadingWhite ~= "" then
				local token = {
					Type = 'Comment',
					CommentType = longStr and 'LongComment' or 'Comment',
					Data = leadingWhite,
					Line = line,
					Char = char,
				}
				token.Print = function()
					return "<"..(token.Type .. string.rep(' ', 7-#token.Type)).."  "..(token.Data or '').." >"
				end
				table.insert(leading, token)
			end

			--get the initial char
			local thisLine = line
			local thisChar = char
			--local errorAt = ":"..line..":"..char..":> "
			local c = peek()

			--symbol to emit
			local toEmit = nil

			--branch on type
			if c == '' then
				--eof
				toEmit = { Type = 'Eof' }

			elseif UpperChars[c] or LowerChars[c] or c == '_' then
				--ident or keyword
				local start = p
				repeat
					get()
					c = peek()
				until not (UpperChars[c] or LowerChars[c] or Digits[c] or c == '_')
				local dat = src:sub(start, p-1)
				if Keywords[dat] then
					toEmit = {Type = 'Keyword', Data = dat}
				else
					toEmit = {Type = 'Ident', Data = dat}
				end

			elseif Digits[c] or (peek() == '.' and Digits[peek(1)]) then
				--number const
				local start = p
				if c == '0' and peek(1) == 'x' then
					get();get()
					while HexDigits[peek()] do get() end
					if consume('Pp') then
						consume('+-')
						while Digits[peek()] do get() end
					end
				else
					while Digits[peek()] do get() end
					if consume('.') then
						while Digits[peek()] do get() end
					end
					if consume('Ee') then
						consume('+-')
						while Digits[peek()] do get() end
					end
				end
				toEmit = {Type = 'Number', Data = src:sub(start, p-1)}

			elseif c == '\'' or c == '\"' then
				local start = p
				--string const
				local delim = get()
				local contentStart = p
				while true do
					local c = get()
					if c == '\\' then
						get() --get the escape char
					elseif c == delim then
						break
					elseif c == '' then
						generateError("Unfinished string near <eof>")
					end
				end
				local content = src:sub(contentStart, p-2)
				local constant = src:sub(start, p-1)
				toEmit = {Type = 'String', Data = constant, Constant = content}

			elseif c == '[' then
				local content, wholetext = tryGetLongString()
				if wholetext then
					toEmit = {Type = 'String', Data = wholetext, Constant = content}
				else
					get()
					toEmit = {Type = 'Symbol', Data = '['}
				end

			elseif consume('>=<') then
				if consume('=') then
					toEmit = {Type = 'Symbol', Data = c..'='}
				else
					toEmit = {Type = 'Symbol', Data = c}
				end

			elseif consume('~') then
				if consume('=') then
					toEmit = {Type = 'Symbol', Data = '~='}
				else
					generateError("Unexpected symbol `~` in source.", 2)
				end

			elseif consume('.') then
				if consume('.') then
					if consume('.') then
						toEmit = {Type = 'Symbol', Data = '...'}
					else
						toEmit = {Type = 'Symbol', Data = '..'}
					end
				else
					toEmit = {Type = 'Symbol', Data = '.'}
				end

			elseif consume(':') then
				if consume(':') then
					toEmit = {Type = 'Symbol', Data = '::'}
				else
					toEmit = {Type = 'Symbol', Data = ':'}
				end

			elseif Symbols[c] then
				get()
				toEmit = {Type = 'Symbol', Data = c}

			else
				local contents, all = tryGetLongString()
				if contents then
					toEmit = {Type = 'String', Data = all, Constant = contents}
				else
					generateError("Unexpected Symbol `"..c.."` in source.", 2)
				end
			end

			--add the emitted symbol, after adding some common data
			toEmit.LeadingWhite = leading -- table of leading whitespace/comments
			--for k, tok in pairs(leading) do
			--	tokens[#tokens + 1] = tok
			--end

			toEmit.Line = thisLine
			toEmit.Char = thisChar
			toEmit.Print = function()
				return "<"..(toEmit.Type..string.rep(' ', 7-#toEmit.Type)).."  "..(toEmit.Data or '').." >"
			end
			tokens[#tokens+1] = toEmit

			--halt after eof has been emitted
			if toEmit.Type == 'Eof' then break end
		end
	end)
	if not st then
		return false, err
	end

	--public interface:
	local tok = {}
	local savedP = {}
	local p = 1

	function tok:getp()
		return p
	end

	function tok:setp(n)
		p = n
	end

	function tok:getTokenList()
		return tokens
	end

	--getters
	function tok:Peek(n)
		n = n or 0
		return tokens[math.min(#tokens, p+n)]
	end
	function tok:Get(tokenList)
		local t = tokens[p]
		p = math.min(p + 1, #tokens)
		if tokenList then
			table.insert(tokenList, t)
		end
		return t
	end
	function tok:Is(t)
		return tok:Peek().Type == t
	end

	--save / restore points in the stream
	function tok:Save()
		savedP[#savedP+1] = p
	end
	function tok:Commit()
		savedP[#savedP] = nil
	end
	function tok:Restore()
		p = savedP[#savedP]
		savedP[#savedP] = nil
	end

	--either return a symbol if there is one, or return true if the requested
	--symbol was gotten.
	function tok:ConsumeSymbol(symb, tokenList)
		local t = self:Peek()
		if t.Type == 'Symbol' then
			if symb then
				if t.Data == symb then
					self:Get(tokenList)
					return true
				else
					return nil
				end
			else
				self:Get(tokenList)
				return t
			end
		else
			return nil
		end
	end

	function tok:ConsumeKeyword(kw, tokenList)
		local t = self:Peek()
		if t.Type == 'Keyword' and t.Data == kw then
			self:Get(tokenList)
			return true
		else
			return nil
		end
	end

	function tok:IsKeyword(kw)
		local t = tok:Peek()
		return t.Type == 'Keyword' and t.Data == kw
	end

	function tok:IsSymbol(s)
		local t = tok:Peek()
		return t.Type == 'Symbol' and t.Data == s
	end

	function tok:IsEof()
		return tok:Peek().Type == 'Eof'
	end

	return true, tok
end


local function ParseLua(src)
	local st, tok
	if type(src) ~= 'table' then
		st, tok = LexLua(src)
	else
		st, tok = true, src
	end
	if not st then
		return false, tok
	end
	--
	local function GenerateError(msg)
		local err = ">> :"..tok:Peek().Line..":"..tok:Peek().Char..": "..msg.."\n"
		--find the line
		local lineNum = 0
		if type(src) == 'string' then
			for line in src:gmatch("[^\n]*\n?") do
				if line:sub(-1,-1) == '\n' then line = line:sub(1,-2) end
				lineNum = lineNum+1
				if lineNum == tok:Peek().Line then
					err = err..">> `"..line:gsub('\t','    ').."`\n"
					for i = 1, tok:Peek().Char do
						local c = line:sub(i,i)
						if c == '\t' then
							err = err..'    '
						else
							err = err..' '
						end
					end
					err = err.."   ^^^^"
					break
				end
			end
		end
		return err
	end
	--
	-- local VarUid = 0
	-- No longer needed: handled in Scopes now local GlobalVarGetMap = {}
	-- local VarDigits = {'_', 'a', 'b', 'c', 'd'}
	local function CreateScope(parent)
		--[[
		local scope = {}
		scope.Parent = parent
		scope.LocalList = {}
		scope.LocalMap = {}

		function scope:ObfuscateVariables()
			for _, var in pairs(scope.LocalList) do
				local id = ""
				repeat
					local chars = "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuioplkjhgfdsazxcvbnm_"
					local chars2 = "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuioplkjhgfdsazxcvbnm_1234567890"
					local n = math.random(1, #chars)
					id = id .. chars:sub(n, n)
					for i = 1, math.random(0,20) do
						local n = math.random(1, #chars2)
						id = id .. chars2:sub(n, n)
					end
				until not GlobalVarGetMap[id] and not parent:GetLocal(id) and not scope.LocalMap[id]
				var.Name = id
				scope.LocalMap[id] = var
			end
		end

		scope.RenameVars = scope.ObfuscateVariables

		-- Renames a variable from this scope and down.
		-- Does not rename global variables.
		function scope:RenameVariable(old, newName)
			if type(old) == "table" then -- its (theoretically) an AstNode variable
				old = old.Name
			end
			for _, var in pairs(scope.LocalList) do
				if var.Name == old then
					var.Name = newName
					scope.LocalMap[newName] = var
				end
			end
		end

		function scope:GetLocal(name)
			--first, try to get my variable
			local my = scope.LocalMap[name]
			if my then return my end

			--next, try parent
			if scope.Parent then
				local par = scope.Parent:GetLocal(name)
				if par then return par end
			end

			return nil
		end

		function scope:CreateLocal(name)
			--create my own var
			local my = {}
			my.Scope = scope
			my.Name = name
			my.CanRename = true
			--
			scope.LocalList[#scope.LocalList+1] = my
			scope.LocalMap[name] = my
			--
			return my
		end]]
		local scope = Scope:new(parent)
		scope.RenameVars = scope.ObfuscateLocals
		scope.ObfuscateVariables = scope.ObfuscateLocals
		scope.Print = function() return "<Scope>" end
		return scope
	end

	local ParseExpr
	local ParseStatementList
	local ParseSimpleExpr,
			ParseSubExpr,
			ParsePrimaryExpr,
			ParseSuffixedExpr

	local function ParseFunctionArgsAndBody(scope, tokenList)
		local funcScope = CreateScope(scope)
		if not tok:ConsumeSymbol('(', tokenList) then
			return false, GenerateError("`(` expected.")
		end

		--arg list
		local argList = {}
		local isVarArg = false
		while not tok:ConsumeSymbol(')', tokenList) do
			if tok:Is('Ident') then
				local arg = funcScope:CreateLocal(tok:Get(tokenList).Data)
				argList[#argList+1] = arg
				if not tok:ConsumeSymbol(',', tokenList) then
					if tok:ConsumeSymbol(')', tokenList) then
						break
					else
						return false, GenerateError("`)` expected.")
					end
				end
			elseif tok:ConsumeSymbol('...', tokenList) then
				isVarArg = true
				if not tok:ConsumeSymbol(')', tokenList) then
					return false, GenerateError("`...` must be the last argument of a function.")
				end
				break
			else
				return false, GenerateError("Argument name or `...` expected")
			end
		end

		--body
		local st, body = ParseStatementList(funcScope)
		if not st then return false, body end

		--end
		if not tok:ConsumeKeyword('end', tokenList) then
			return false, GenerateError("`end` expected after function body")
		end
		local nodeFunc = {}
		nodeFunc.AstType   = 'Function'
		nodeFunc.Scope     = funcScope
		nodeFunc.Arguments = argList
		nodeFunc.Body      = body
		nodeFunc.VarArg    = isVarArg
		nodeFunc.Tokens    = tokenList
		--
		return true, nodeFunc
	end
)lua_codes"

R"lua_codes(
	function ParsePrimaryExpr(scope)
		local tokenList = {}

		if tok:ConsumeSymbol('(', tokenList) then
			local st, ex = ParseExpr(scope)
			if not st then return false, ex end
			if not tok:ConsumeSymbol(')', tokenList) then
				return false, GenerateError("`)` Expected.")
			end
			if false then
				--save the information about parenthesized expressions somewhere
				ex.ParenCount = (ex.ParenCount or 0) + 1
				return true, ex
			else
				local parensExp = {}
				parensExp.AstType   = 'Parentheses'
				parensExp.Inner     = ex
				parensExp.Tokens    = tokenList
				return true, parensExp
			end

		elseif tok:Is('Ident') then
			local id = tok:Get(tokenList)
			local var = scope:GetLocal(id.Data)
			if not var then
				var = scope:GetGlobal(id.Data)
				if not var then
					var = scope:CreateGlobal(id.Data)
				else
					var.References = var.References + 1
				end
			else
				var.References = var.References + 1
			end
			--
			local nodePrimExp = {}
			nodePrimExp.AstType   = 'VarExpr'
			nodePrimExp.Name      = id.Data
			nodePrimExp.Variable  = var
			nodePrimExp.Tokens    = tokenList
			--
			return true, nodePrimExp
		else
			return false, GenerateError("primary expression expected")
		end
	end

	function ParseSuffixedExpr(scope, onlyDotColon)
		--base primary expression
		local st, prim = ParsePrimaryExpr(scope)
		if not st then return false, prim end
		--
		while true do
			local tokenList = {}

			if tok:IsSymbol('.') or tok:IsSymbol(':') then
				local symb = tok:Get(tokenList).Data
				if not tok:Is('Ident') then
					return false, GenerateError("<Ident> expected.")
				end
				local id = tok:Get(tokenList)
				local nodeIndex = {}
				nodeIndex.AstType  = 'MemberExpr'
				nodeIndex.Base     = prim
				nodeIndex.Indexer  = symb
				nodeIndex.Ident    = id
				nodeIndex.Tokens   = tokenList
				--
				prim = nodeIndex

			elseif not onlyDotColon and tok:ConsumeSymbol('[', tokenList) then
				local st, ex = ParseExpr(scope)
				if not st then return false, ex end
				if not tok:ConsumeSymbol(']', tokenList) then
					return false, GenerateError("`]` expected.")
				end
				local nodeIndex = {}
				nodeIndex.AstType  = 'IndexExpr'
				nodeIndex.Base     = prim
				nodeIndex.Index    = ex
				nodeIndex.Tokens   = tokenList
				--
				prim = nodeIndex

			elseif not onlyDotColon and tok:ConsumeSymbol('(', tokenList) then
				local args = {}
				while not tok:ConsumeSymbol(')', tokenList) do
					local st, ex = ParseExpr(scope)
					if not st then return false, ex end
					args[#args+1] = ex
					if not tok:ConsumeSymbol(',', tokenList) then
						if tok:ConsumeSymbol(')', tokenList) then
							break
						else
							return false, GenerateError("`)` Expected.")
						end
					end
				end
				local nodeCall = {}
				nodeCall.AstType   = 'CallExpr'
				nodeCall.Base      = prim
				nodeCall.Arguments = args
				nodeCall.Tokens    = tokenList
				--
				prim = nodeCall

			elseif not onlyDotColon and tok:Is('String') then
				--string call
				local nodeCall = {}
				nodeCall.AstType    = 'StringCallExpr'
				nodeCall.Base       = prim
				nodeCall.Arguments  = { tok:Get(tokenList) }
				nodeCall.Tokens     = tokenList
				--
				prim = nodeCall

			elseif not onlyDotColon and tok:IsSymbol('{') then
				--table call
				local st, ex = ParseSimpleExpr(scope)
				-- FIX: ParseExpr(scope) parses the table AND and any following binary expressions.
				-- We just want the table
				if not st then return false, ex end
				local nodeCall = {}
				nodeCall.AstType   = 'TableCallExpr'
				nodeCall.Base      = prim
				nodeCall.Arguments = { ex }
				nodeCall.Tokens    = tokenList
				--
				prim = nodeCall

			else
				break
			end
		end
		return true, prim
	end


	function ParseSimpleExpr(scope)
		local tokenList = {}

		if tok:Is('Number') then
			local nodeNum = {}
			nodeNum.AstType = 'NumberExpr'
			nodeNum.Value   = tok:Get(tokenList)
			nodeNum.Tokens  = tokenList
			return true, nodeNum

		elseif tok:Is('String') then
			local nodeStr = {}
			nodeStr.AstType = 'StringExpr'
			nodeStr.Value   = tok:Get(tokenList)
			nodeStr.Tokens  = tokenList
			return true, nodeStr

		elseif tok:ConsumeKeyword('nil', tokenList) then
			local nodeNil = {}
			nodeNil.AstType = 'NilExpr'
			nodeNil.Tokens  = tokenList
			return true, nodeNil

		elseif tok:IsKeyword('false') or tok:IsKeyword('true') then
			local nodeBoolean = {}
			nodeBoolean.AstType = 'BooleanExpr'
			nodeBoolean.Value   = (tok:Get(tokenList).Data == 'true')
			nodeBoolean.Tokens  = tokenList
			return true, nodeBoolean

		elseif tok:ConsumeSymbol('...', tokenList) then
			local nodeDots = {}
			nodeDots.AstType  = 'DotsExpr'
			nodeDots.Tokens   = tokenList
			return true, nodeDots

		elseif tok:ConsumeSymbol('{', tokenList) then
			local v = {}
			v.AstType = 'ConstructorExpr'
			v.EntryList = {}
			--
			while true do
				if tok:IsSymbol('[', tokenList) then
					--key
					tok:Get(tokenList)
					local st, key = ParseExpr(scope)
					if not st then
						return false, GenerateError("Key Expression Expected")
					end
					if not tok:ConsumeSymbol(']', tokenList) then
						return false, GenerateError("`]` Expected")
					end
					if not tok:ConsumeSymbol('=', tokenList) then
						return false, GenerateError("`=` Expected")
					end
					local st, value = ParseExpr(scope)
					if not st then
						return false, GenerateError("Value Expression Expected")
					end
					v.EntryList[#v.EntryList+1] = {
						Type  = 'Key';
						Key   = key;
						Value = value;
					}

				elseif tok:Is('Ident') then
					--value or key
					local lookahead = tok:Peek(1)
					if lookahead.Type == 'Symbol' and lookahead.Data == '=' then
						--we are a key
						local key = tok:Get(tokenList)
						if not tok:ConsumeSymbol('=', tokenList) then
							return false, GenerateError("`=` Expected")
						end
						local st, value = ParseExpr(scope)
						if not st then
							return false, GenerateError("Value Expression Expected")
						end
						v.EntryList[#v.EntryList+1] = {
							Type  = 'KeyString';
							Key   = key.Data;
							Value = value;
						}

					else
						--we are a value
						local st, value = ParseExpr(scope)
						if not st then
							return false, GenerateError("Value Exected")
						end
						v.EntryList[#v.EntryList+1] = {
							Type = 'Value';
							Value = value;
						}

					end
				elseif tok:ConsumeSymbol('}', tokenList) then
					break

				else
					--value
					local st, value = ParseExpr(scope)
					v.EntryList[#v.EntryList+1] = {
						Type = 'Value';
						Value = value;
					}
					if not st then
						return false, GenerateError("Value Expected")
					end
				end

				if tok:ConsumeSymbol(';', tokenList) or tok:ConsumeSymbol(',', tokenList) then
					--all is good
				elseif tok:ConsumeSymbol('}', tokenList) then
					break
				else
					return false, GenerateError("`}` or table entry Expected")
				end
			end
			v.Tokens  = tokenList
			return true, v

		elseif tok:ConsumeKeyword('function', tokenList) then
			local st, func = ParseFunctionArgsAndBody(scope, tokenList)
			if not st then return false, func end
			--
			func.IsLocal = true
			return true, func

		else
			return ParseSuffixedExpr(scope)
		end
	end


	local unops = lookupify{'-', 'not', '#'}
	local unopprio = 8
	local priority = {
		['+'] = {6,6};
		['-'] = {6,6};
		['%'] = {7,7};
		['/'] = {7,7};
		['*'] = {7,7};
		['^'] = {10,9};
		['..'] = {5,4};
		['=='] = {3,3};
		['<'] = {3,3};
		['<='] = {3,3};
		['~='] = {3,3};
		['>'] = {3,3};
		['>='] = {3,3};
		['and'] = {2,2};
		['or'] = {1,1};
	}
	function ParseSubExpr(scope, level)
		--base item, possibly with unop prefix
		local st, exp
		if unops[tok:Peek().Data] then
			local tokenList = {}
			local op = tok:Get(tokenList).Data
			st, exp = ParseSubExpr(scope, unopprio)
			if not st then return false, exp end
			local nodeEx = {}
			nodeEx.AstType = 'UnopExpr'
			nodeEx.Rhs     = exp
			nodeEx.Op      = op
			nodeEx.OperatorPrecedence = unopprio
			nodeEx.Tokens  = tokenList
			exp = nodeEx
		else
			st, exp = ParseSimpleExpr(scope)
			if not st then return false, exp end
		end

		--next items in chain
		while true do
			local prio = priority[tok:Peek().Data]
			if prio and prio[1] > level then
				local tokenList = {}
				local op = tok:Get(tokenList).Data
				local st, rhs = ParseSubExpr(scope, prio[2])
				if not st then return false, rhs end
				local nodeEx = {}
				nodeEx.AstType = 'BinopExpr'
				nodeEx.Lhs     = exp
				nodeEx.Op      = op
				nodeEx.OperatorPrecedence = prio[1]
				nodeEx.Rhs     = rhs
				nodeEx.Tokens  = tokenList
				--
				exp = nodeEx
			else
				break
			end
		end

		return true, exp
	end


	ParseExpr = function(scope)
		return ParseSubExpr(scope, 0)
	end
)lua_codes"

R"lua_codes(
	local function ParseStatement(scope)
		local stat = nil
		local tokenList = {}
		if tok:ConsumeKeyword('if', tokenList) then
			--setup
			local nodeIfStat = {}
			nodeIfStat.AstType = 'IfStatement'
			nodeIfStat.Clauses = {}

			--clauses
			repeat
				local st, nodeCond = ParseExpr(scope)
				if not st then return false, nodeCond end
				if not tok:ConsumeKeyword('then', tokenList) then
					return false, GenerateError("`then` expected.")
				end
				local st, nodeBody = ParseStatementList(scope)
				if not st then return false, nodeBody end
				nodeIfStat.Clauses[#nodeIfStat.Clauses+1] = {
					Condition = nodeCond;
					Body = nodeBody;
				}
			until not tok:ConsumeKeyword('elseif', tokenList)

			--else clause
			if tok:ConsumeKeyword('else', tokenList) then
				local st, nodeBody = ParseStatementList(scope)
				if not st then return false, nodeBody end
				nodeIfStat.Clauses[#nodeIfStat.Clauses+1] = {
					Body = nodeBody;
				}
			end

			--end
			if not tok:ConsumeKeyword('end', tokenList) then
				return false, GenerateError("`end` expected.")
			end

			nodeIfStat.Tokens = tokenList
			stat = nodeIfStat

		elseif tok:ConsumeKeyword('while', tokenList) then
			--setup
			local nodeWhileStat = {}
			nodeWhileStat.AstType = 'WhileStatement'

			--condition
			local st, nodeCond = ParseExpr(scope)
			if not st then return false, nodeCond end

			--do
			if not tok:ConsumeKeyword('do', tokenList) then
				return false, GenerateError("`do` expected.")
			end

			--body
			local st, nodeBody = ParseStatementList(scope)
			if not st then return false, nodeBody end

			--end
			if not tok:ConsumeKeyword('end', tokenList) then
				return false, GenerateError("`end` expected.")
			end

			--return
			nodeWhileStat.Condition = nodeCond
			nodeWhileStat.Body      = nodeBody
			nodeWhileStat.Tokens    = tokenList
			stat = nodeWhileStat

		elseif tok:ConsumeKeyword('do', tokenList) then
			--do block
			local st, nodeBlock = ParseStatementList(scope)
			if not st then return false, nodeBlock end
			if not tok:ConsumeKeyword('end', tokenList) then
				return false, GenerateError("`end` expected.")
			end

			local nodeDoStat = {}
			nodeDoStat.AstType = 'DoStatement'
			nodeDoStat.Body    = nodeBlock
			nodeDoStat.Tokens  = tokenList
			stat = nodeDoStat

		elseif tok:ConsumeKeyword('for', tokenList) then
			--for block
			if not tok:Is('Ident') then
				return false, GenerateError("<ident> expected.")
			end
			local baseVarName = tok:Get(tokenList)
			if tok:ConsumeSymbol('=', tokenList) then
				--numeric for
				local forScope = CreateScope(scope)
				local forVar = forScope:CreateLocal(baseVarName.Data)
				--
				local st, startEx = ParseExpr(scope)
				if not st then return false, startEx end
				if not tok:ConsumeSymbol(',', tokenList) then
					return false, GenerateError("`,` Expected")
				end
				local st, endEx = ParseExpr(scope)
				if not st then return false, endEx end
				local st, stepEx;
				if tok:ConsumeSymbol(',', tokenList) then
					st, stepEx = ParseExpr(scope)
					if not st then return false, stepEx end
				end
				if not tok:ConsumeKeyword('do', tokenList) then
					return false, GenerateError("`do` expected")
				end
				--
				local st, body = ParseStatementList(forScope)
				if not st then return false, body end
				if not tok:ConsumeKeyword('end', tokenList) then
					return false, GenerateError("`end` expected")
				end
				--
				local nodeFor = {}
				nodeFor.AstType  = 'NumericForStatement'
				nodeFor.Scope    = forScope
				nodeFor.Variable = forVar
				nodeFor.Start    = startEx
				nodeFor.End      = endEx
				nodeFor.Step     = stepEx
				nodeFor.Body     = body
				nodeFor.Tokens   = tokenList
				stat = nodeFor
			else
				--generic for
				local forScope = CreateScope(scope)
				--
				local varList = { forScope:CreateLocal(baseVarName.Data) }
				while tok:ConsumeSymbol(',', tokenList) do
					if not tok:Is('Ident') then
						return false, GenerateError("for variable expected.")
					end
					varList[#varList+1] = forScope:CreateLocal(tok:Get(tokenList).Data)
				end
				if not tok:ConsumeKeyword('in', tokenList) then
					return false, GenerateError("`in` expected.")
				end
				local generators = {}
				local st, firstGenerator = ParseExpr(scope)
				if not st then return false, firstGenerator end
				generators[#generators+1] = firstGenerator
				while tok:ConsumeSymbol(',', tokenList) do
					local st, gen = ParseExpr(scope)
					if not st then return false, gen end
					generators[#generators+1] = gen
				end
				if not tok:ConsumeKeyword('do', tokenList) then
					return false, GenerateError("`do` expected.")
				end
				local st, body = ParseStatementList(forScope)
				if not st then return false, body end
				if not tok:ConsumeKeyword('end', tokenList) then
					return false, GenerateError("`end` expected.")
				end
				--
				local nodeFor = {}
				nodeFor.AstType      = 'GenericForStatement'
				nodeFor.Scope        = forScope
				nodeFor.VariableList = varList
				nodeFor.Generators   = generators
				nodeFor.Body         = body
				nodeFor.Tokens       = tokenList
				stat = nodeFor
			end

		elseif tok:ConsumeKeyword('repeat', tokenList) then
			local st, body = ParseStatementList(scope)
			if not st then return false, body end
			--
			if not tok:ConsumeKeyword('until', tokenList) then
				return false, GenerateError("`until` expected.")
			end
			-- FIX: Used to parse in parent scope
			-- Now parses in repeat scope
			local st, cond = ParseExpr(body.Scope)
			if not st then return false, cond end
			--
			local nodeRepeat = {}
			nodeRepeat.AstType   = 'RepeatStatement'
			nodeRepeat.Condition = cond
			nodeRepeat.Body      = body
			nodeRepeat.Tokens    = tokenList
			stat = nodeRepeat

		elseif tok:ConsumeKeyword('function', tokenList) then
			if not tok:Is('Ident') then
				return false, GenerateError("Function name expected")
			end
			local st, name = ParseSuffixedExpr(scope, true) --true => only dots and colons
			if not st then return false, name end
			--
			local st, func = ParseFunctionArgsAndBody(scope, tokenList)
			if not st then return false, func end
			--
			func.IsLocal = false
			func.Name    = name
			stat = func

		elseif tok:ConsumeKeyword('local', tokenList) then
			if tok:Is('Ident') then
				local varList, attrList = {}, {}
				repeat
					if not tok:Is('Ident') then
						return false, GenerateError("local var name expected")
					end
					varList[#varList+1] = tok:Get(tokenList).Data
					if tok:ConsumeSymbol('<', tokenList) then
						if not tok:Is('Ident') then
							return false, GenerateError("attrib name expected")
						end
						attrList[#attrList+1] = tok:Get(tokenList).Data
						if not tok:ConsumeSymbol('>', tokenList) then
							return false, GenerateError("missing '>' to close attrib name")
						end
					else
						attrList[#attrList+1] = false
					end
				until not tok:ConsumeSymbol(',', tokenList)

				local initList = {}
				if tok:ConsumeSymbol('=', tokenList) then
					repeat
						local st, ex = ParseExpr(scope)
						if not st then return false, ex end
						initList[#initList+1] = ex
					until not tok:ConsumeSymbol(',', tokenList)
				end

				--now patch var list
				--we can't do this before getting the init list, because the init list does not
				--have the locals themselves in scope.
				for i, v in pairs(varList) do
					varList[i] = scope:CreateLocal(v)
				end

				local nodeLocal = {}
				nodeLocal.AstType   = 'LocalStatement'
				nodeLocal.LocalList = varList
				nodeLocal.AttrList  = attrList
				nodeLocal.InitList  = initList
				nodeLocal.Tokens    = tokenList
				--
				stat = nodeLocal

			elseif tok:ConsumeKeyword('function', tokenList) then
				if not tok:Is('Ident') then
					return false, GenerateError("Function name expected")
				end
				local name = tok:Get(tokenList).Data
				local localVar = scope:CreateLocal(name)
				--
				local st, func = ParseFunctionArgsAndBody(scope, tokenList)
				if not st then return false, func end
				--
				func.Name         = localVar
				func.IsLocal      = true
				stat = func

			else
				return false, GenerateError("local var or function def expected")
			end

		elseif tok:ConsumeSymbol('::', tokenList) then
			if not tok:Is('Ident') then
				return false, GenerateError('Label name expected')
			end
			local label = tok:Get(tokenList).Data
			if not tok:ConsumeSymbol('::', tokenList) then
				return false, GenerateError("`::` expected")
			end
			local nodeLabel = {}
			nodeLabel.AstType = 'LabelStatement'
			nodeLabel.Label   = label
			nodeLabel.Tokens  = tokenList
			stat = nodeLabel

		elseif tok:ConsumeKeyword('return', tokenList) then
			local exList = {}
			if not tok:IsKeyword('end') then
				local st, firstEx = ParseExpr(scope)
				if st then
					exList[1] = firstEx
					while tok:ConsumeSymbol(',', tokenList) do
						local st, ex = ParseExpr(scope)
						if not st then return false, ex end
						exList[#exList+1] = ex
					end
				end
			end

			local nodeReturn = {}
			nodeReturn.AstType   = 'ReturnStatement'
			nodeReturn.Arguments = exList
			nodeReturn.Tokens    = tokenList
			stat = nodeReturn

		elseif tok:ConsumeKeyword('break', tokenList) then
			local nodeBreak = {}
			nodeBreak.AstType = 'BreakStatement'
			nodeBreak.Tokens  = tokenList
			stat = nodeBreak

		elseif tok:ConsumeKeyword('goto', tokenList) then
			if not tok:Is('Ident') then
				return false, GenerateError("Label expected")
			end
			local label = tok:Get(tokenList).Data
			local nodeGoto = {}
			nodeGoto.AstType = 'GotoStatement'
			nodeGoto.Label   = label
			nodeGoto.Tokens  = tokenList
			stat = nodeGoto

		else
			--statementParseExpr
			local st, suffixed = ParseSuffixedExpr(scope)
			if not st then return false, suffixed end

			--assignment or call?
			if tok:IsSymbol(',') or tok:IsSymbol('=') then
				--check that it was not parenthesized, making it not an lvalue
				if (suffixed.ParenCount or 0) > 0 then
					return false, GenerateError("Can not assign to parenthesized expression, is not an lvalue")
				end

				--more processing needed
				local lhs = { suffixed }
				while tok:ConsumeSymbol(',', tokenList) do
					local st, lhsPart = ParseSuffixedExpr(scope)
					if not st then return false, lhsPart end
					lhs[#lhs+1] = lhsPart
				end

				--equals
				if not tok:ConsumeSymbol('=', tokenList) then
					return false, GenerateError("`=` Expected.")
				end

				--rhs
				local rhs = {}
				local st, firstRhs = ParseExpr(scope)
				if not st then return false, firstRhs end
				rhs[1] = firstRhs
				while tok:ConsumeSymbol(',', tokenList) do
					local st, rhsPart = ParseExpr(scope)
					if not st then return false, rhsPart end
					rhs[#rhs+1] = rhsPart
				end

				--done
				local nodeAssign = {}
				nodeAssign.AstType = 'AssignmentStatement'
				nodeAssign.Lhs     = lhs
				nodeAssign.Rhs     = rhs
				nodeAssign.Tokens  = tokenList
				stat = nodeAssign

			elseif suffixed.AstType == 'CallExpr' or
				   suffixed.AstType == 'TableCallExpr' or
				   suffixed.AstType == 'StringCallExpr'
			then
				--it's a call statement
				local nodeCall = {}
				nodeCall.AstType    = 'CallStatement'
				nodeCall.Expression = suffixed
				nodeCall.Tokens     = tokenList
				stat = nodeCall
			else
				return false, GenerateError("Assignment Statement Expected")
			end
		end

		if tok:IsSymbol(';') then
			stat.Semicolon = tok:Get( stat.Tokens )
		end
		return true, stat
	end


	local statListCloseKeywords = lookupify{'end', 'else', 'elseif', 'until'}

	ParseStatementList = function(scope)
		local nodeStatlist   = {}
		nodeStatlist.Scope   = CreateScope(scope)
		nodeStatlist.AstType = 'Statlist'
		nodeStatlist.Body    = { }
		nodeStatlist.Tokens  = { }
		--
		--local stats = {}
		--
		while not statListCloseKeywords[tok:Peek().Data] and not tok:IsEof() do
			local st, nodeStatement = ParseStatement(nodeStatlist.Scope)
			if not st then return false, nodeStatement end
			--stats[#stats+1] = nodeStatement
			nodeStatlist.Body[#nodeStatlist.Body + 1] = nodeStatement
		end

		if tok:IsEof() then
			local nodeEof = {}
			nodeEof.AstType = 'Eof'
			nodeEof.Tokens  = { tok:Get() }
			nodeStatlist.Body[#nodeStatlist.Body + 1] = nodeEof
		end

		--
		--nodeStatlist.Body = stats
		return true, nodeStatlist
	end


	local function mainfunc()
		local topScope = CreateScope()
		return ParseStatementList(topScope)
	end

	local st, main = mainfunc()
	--print("Last Token: "..PrintTable(tok:Peek()))
	return st, main
end
)lua_codes"

R"lua_codes(
--
-- FormatMini.lua
--
-- Returns the minified version of an AST. Operations which are performed:
-- - All comments and whitespace are ignored
-- - All local variables are renamed
--

local function Format_Mini(ast)
	local formatStatlist, formatExpr;
	--local count = 0
	--
	local function joinStatementsSafe(a, b, sep)
	--print(a, b)
		--[[
		if count > 150 then
			count = 0
			return a.."\n"..b
		end]]
		sep = sep or ' '
		if sep == ';' then
			local token = a:match("([%w_]+)%s*$")
			if token == "then" or token == "do" then
				sep = ' '
			end
		end
		local aa, bb = a:sub(-1,-1), b:sub(1,1)
		if UpperChars[aa] or LowerChars[aa] or aa == '_' then
			if not (UpperChars[bb] or LowerChars[bb] or bb == '_' or Digits[bb]) then
				--bb is a symbol, can join without sep
				return a..b
			elseif bb == '(' then
				--prevent ambiguous syntax
				return a..sep..b
			else
				return a..sep..b
			end
		elseif Digits[aa] then
			if bb == '(' then
				--can join statements directly
				return a..b
			elseif Symbols[bb] then
				return a .. b
			else
				return a..sep..b
			end
		elseif aa == '' then
			return a..b
		else
			if bb == '(' then
				--don't want to accidentally call last statement, can't join directly
				return a..sep..b
			else
			--print("asdf", '"'..a..'"', '"'..b..'"')
				return a..b
			end
		end
	end

	formatExpr = function(expr, precedence)
		local precedence = precedence or 0
		local currentPrecedence = 0
		local skipParens = false
		local out = ""
		if expr.AstType == 'VarExpr' then
			if expr.Variable then
				out = out..expr.Variable.Name
			else
				out = out..expr.Name
			end

		elseif expr.AstType == 'NumberExpr' then
			out = out..expr.Value.Data

		elseif expr.AstType == 'StringExpr' then
			out = out..expr.Value.Data

		elseif expr.AstType == 'BooleanExpr' then
			out = out..tostring(expr.Value)

		elseif expr.AstType == 'NilExpr' then
			out = joinStatementsSafe(out, "nil")

		elseif expr.AstType == 'BinopExpr' then
			currentPrecedence = expr.OperatorPrecedence
			out = joinStatementsSafe(out, formatExpr(expr.Lhs, currentPrecedence))
			out = joinStatementsSafe(out, expr.Op)
			out = joinStatementsSafe(out, formatExpr(expr.Rhs))
			if expr.Op == '^' or expr.Op == '..' then
				currentPrecedence = currentPrecedence - 1
			end

			if currentPrecedence < precedence then
				skipParens = false
			else
				skipParens = true
			end
			--print(skipParens, precedence, currentPrecedence)
		elseif expr.AstType == 'UnopExpr' then
			out = joinStatementsSafe(out, expr.Op)
			out = joinStatementsSafe(out, formatExpr(expr.Rhs))

		elseif expr.AstType == 'DotsExpr' then
			out = out.."..."

		elseif expr.AstType == 'CallExpr' then
			out = out..formatExpr(expr.Base)
			out = out.."("
			for i = 1, #expr.Arguments do
				out = out..formatExpr(expr.Arguments[i])
				if i ~= #expr.Arguments then
					out = out..","
				end
			end
			out = out..")"

		elseif expr.AstType == 'TableCallExpr' then
			out = out..formatExpr(expr.Base)
			out = out..formatExpr(expr.Arguments[1])

		elseif expr.AstType == 'StringCallExpr' then
			out = out..formatExpr(expr.Base)
			out = out..expr.Arguments[1].Data

		elseif expr.AstType == 'IndexExpr' then
			out = out..formatExpr(expr.Base).."["..formatExpr(expr.Index).."]"

		elseif expr.AstType == 'MemberExpr' then
			out = out..formatExpr(expr.Base)..expr.Indexer..expr.Ident.Data

		elseif expr.AstType == 'Function' then
			expr.Scope:ObfuscateVariables()
			out = out.."function("
			if #expr.Arguments > 0 then
				for i = 1, #expr.Arguments do
					out = out..expr.Arguments[i].Name
					if i ~= #expr.Arguments then
						out = out..","
					elseif expr.VarArg then
						out = out..",..."
					end
				end
			elseif expr.VarArg then
				out = out.."..."
			end
			out = out..")"
			out = joinStatementsSafe(out, formatStatlist(expr.Body))
			out = joinStatementsSafe(out, "end")

		elseif expr.AstType == 'ConstructorExpr' then
			out = out.."{"
			for i = 1, #expr.EntryList do
				local entry = expr.EntryList[i]
				if entry.Type == 'Key' then
					out = out.."["..formatExpr(entry.Key).."]="..formatExpr(entry.Value)
				elseif entry.Type == 'Value' then
					out = out..formatExpr(entry.Value)
				elseif entry.Type == 'KeyString' then
					out = out..entry.Key.."="..formatExpr(entry.Value)
				end
				if i ~= #expr.EntryList then
					out = out..","
				end
			end
			out = out.."}"

		elseif expr.AstType == 'Parentheses' then
			out = out.."("..formatExpr(expr.Inner)..")"

		end
		--print(">>", skipParens, expr.ParenCount, out)
		if not skipParens then
			--print("hehe")
			out = string.rep('(', expr.ParenCount or 0) .. out
			out = out .. string.rep(')', expr.ParenCount or 0)
			--print("", out)
		end
		--count = count + #out
		return --[[print(out) or]] out
	end

	local formatStatement = function(statement)
		local out = ''
		if statement.AstType == 'AssignmentStatement' then
			for i = 1, #statement.Lhs do
				out = out..formatExpr(statement.Lhs[i])
				if i ~= #statement.Lhs then
					out = out..","
				end
			end
			if #statement.Rhs > 0 then
				out = out.."="
				for i = 1, #statement.Rhs do
					out = out..formatExpr(statement.Rhs[i])
					if i ~= #statement.Rhs then
						out = out..","
					end
				end
			end

		elseif statement.AstType == 'CallStatement' then
			out = formatExpr(statement.Expression)

		elseif statement.AstType == 'LocalStatement' then
			out = out.."local "
			for i = 1, #statement.LocalList do
				out = out..statement.LocalList[i].Name
				if statement.AttrList[i] then
					out = out.."<"..statement.AttrList[i]..">"
					if i == #statement.LocalList then
						out = out.." "
					end
				end
				if i ~= #statement.LocalList then
					out = out..","
				end
			end
			if #statement.InitList > 0 then
				out = out.."="
				for i = 1, #statement.InitList do
					out = out..formatExpr(statement.InitList[i])
					if i ~= #statement.InitList then
						out = out..","
					end
				end
			end

		elseif statement.AstType == 'IfStatement' then
			out = joinStatementsSafe("if", formatExpr(statement.Clauses[1].Condition))
			out = joinStatementsSafe(out, "then")
			out = joinStatementsSafe(out, formatStatlist(statement.Clauses[1].Body))
			for i = 2, #statement.Clauses do
				local st = statement.Clauses[i]
				if st.Condition then
					out = joinStatementsSafe(out, "elseif")
					out = joinStatementsSafe(out, formatExpr(st.Condition))
					out = joinStatementsSafe(out, "then")
				else
					out = joinStatementsSafe(out, "else")
				end
				out = joinStatementsSafe(out, formatStatlist(st.Body))
			end
			out = joinStatementsSafe(out, "end")

		elseif statement.AstType == 'WhileStatement' then
			out = joinStatementsSafe("while", formatExpr(statement.Condition))
			out = joinStatementsSafe(out, "do")
			out = joinStatementsSafe(out, formatStatlist(statement.Body))
			out = joinStatementsSafe(out, "end")

		elseif statement.AstType == 'DoStatement' then
			out = joinStatementsSafe(out, "do")
			out = joinStatementsSafe(out, formatStatlist(statement.Body))
			out = joinStatementsSafe(out, "end")

		elseif statement.AstType == 'ReturnStatement' then
			out = "return"
			for i = 1, #statement.Arguments do
				out = joinStatementsSafe(out, formatExpr(statement.Arguments[i]))
				if i ~= #statement.Arguments then
					out = out..","
				end
			end

		elseif statement.AstType == 'BreakStatement' then
			out = "break"

		elseif statement.AstType == 'RepeatStatement' then
			out = "repeat"
			out = joinStatementsSafe(out, formatStatlist(statement.Body))
			out = joinStatementsSafe(out, "until")
			out = joinStatementsSafe(out, formatExpr(statement.Condition))

		elseif statement.AstType == 'Function' then
			statement.Scope:ObfuscateVariables()
			if statement.IsLocal then
				out = "local"
			end
			out = joinStatementsSafe(out, "function ")
			if statement.IsLocal then
				out = out..statement.Name.Name
			else
				out = out..formatExpr(statement.Name)
			end
			out = out.."("
			if #statement.Arguments > 0 then
				for i = 1, #statement.Arguments do
					out = out..statement.Arguments[i].Name
					if i ~= #statement.Arguments then
						out = out..","
					elseif statement.VarArg then
						--print("Apply vararg")
						out = out..",..."
					end
				end
			elseif statement.VarArg then
				out = out.."..."
			end
			out = out..")"
			out = joinStatementsSafe(out, formatStatlist(statement.Body))
			out = joinStatementsSafe(out, "end")

		elseif statement.AstType == 'GenericForStatement' then
			statement.Scope:ObfuscateVariables()
			out = "for "
			for i = 1, #statement.VariableList do
				out = out..statement.VariableList[i].Name
				if i ~= #statement.VariableList then
					out = out..","
				end
			end
			out = out.." in"
			for i = 1, #statement.Generators do
				out = joinStatementsSafe(out, formatExpr(statement.Generators[i]))
				if i ~= #statement.Generators then
					out = joinStatementsSafe(out, ',')
				end
			end
			out = joinStatementsSafe(out, "do")
			out = joinStatementsSafe(out, formatStatlist(statement.Body))
			out = joinStatementsSafe(out, "end")

		elseif statement.AstType == 'NumericForStatement' then
			statement.Scope:ObfuscateVariables()
			out = "for "
			out = out..statement.Variable.Name.."="
			out = out..formatExpr(statement.Start)..","..formatExpr(statement.End)
			if statement.Step then
				out = out..","..formatExpr(statement.Step)
			end
			out = joinStatementsSafe(out, "do")
			out = joinStatementsSafe(out, formatStatlist(statement.Body))
			out = joinStatementsSafe(out, "end")
		elseif statement.AstType == 'LabelStatement' then
			out = joinStatementsSafe(out, "::" .. statement.Label .. "::")
		elseif statement.AstType == 'GotoStatement' then
			out = joinStatementsSafe(out, "goto " .. statement.Label)
		elseif statement.AstType == 'Comment' then
			-- ignore
		elseif statement.AstType == 'Eof' then
			-- ignore
		else
			print("Unknown AST Type: " .. statement.AstType)
		end
		--count = count + #out
		return out
	end

	formatStatlist = function(statList)
		local out = ''
		statList.Scope:ObfuscateVariables()
		for _, stat in pairs(statList.Body) do
			out = joinStatementsSafe(out, formatStatement(stat), ';')
		end
		return out
	end

	ast.Scope:ObfuscateVariables()
	return formatStatlist(ast)
end
)lua_codes"

R"lua_codes(
local function FormatYue(ast, lineMap)
	local currentLine = 1
	local formatStatlist, formatExpr

	local function joinStatementsSafe(out, b, sep)
		if #out < 1 then
			return ''
		end
		local aa = ''
		local b1 = b:sub(1,1)
		local spaceSep = b1 == ' ' or b1 == '\n'
		for i = #out, 1, -1 do
			local a = out[i]
			local a1 = a:sub(-1,-1)
			if a1 == ' ' or a1 == '\n' then
				spaceSep = true
			end
			aa = a:match("([^%s])%s*$")
			if aa then
				break
			end
		end
		aa = aa or ''
		sep = sep or ' '
		if spaceSep then
			sep = ''
		elseif sep == ';' then
			local token = aa:match("([%w_]+)%s*$")
			if token == "then" or token == "do" then
				sep = ' '
			end
		end
		local bb = b:match("^%s*([^%s])")
		if UpperChars[aa] or LowerChars[aa] or aa == '_' then
			if not (UpperChars[bb] or LowerChars[bb] or bb == '_' or Digits[bb]) then
				--bb is a symbol, can join without sep
				out[#out + 1] = b
			elseif bb == '(' then
				--prevent ambiguous syntax
				out[#out + 1] = sep
				out[#out + 1] = b
			else
				out[#out + 1] = sep
				out[#out + 1] = b
			end
		elseif Digits[aa] then
			if bb == '(' then
				--can join statements directly
				out[#out + 1] = b
			elseif Symbols[bb] then
				out[#out + 1] = b
			else
				out[#out + 1] = sep
				out[#out + 1] = b
			end
		elseif aa == '' then
			out[#out + 1] = b
		else
			if bb == '(' then
				--don't want to accidentally call last statement, can't join directly
				out[#out + 1] = sep
				out[#out + 1] = b
			else
				out[#out + 1] = b
			end
		end
	end

	formatExpr = function(expr)
		local out = {string.rep('(', expr.ParenCount or 0)}
		if expr.AstType == 'VarExpr' then
			if expr.Variable then
				out[#out + 1] = expr.Variable.Name
			else
				out[#out + 1] = expr.Name
			end

		elseif expr.AstType == 'NumberExpr' then
			out[#out + 1] = expr.Value.Data

		elseif expr.AstType == 'StringExpr' then
			out[#out + 1] = expr.Value.Data

		elseif expr.AstType == 'BooleanExpr' then
			out[#out + 1] = tostring(expr.Value)

		elseif expr.AstType == 'NilExpr' then
			joinStatementsSafe(out, "nil", nil)

		elseif expr.AstType == 'BinopExpr' then
			joinStatementsSafe(out, formatExpr(expr.Lhs), nil)
			out[#out + 1] = " "
			joinStatementsSafe(out, expr.Op, nil)
			out[#out + 1] = " "
			joinStatementsSafe(out, formatExpr(expr.Rhs), nil)

		elseif expr.AstType == 'UnopExpr' then
			joinStatementsSafe(out, expr.Op, nil)
			out[#out + 1] = (#expr.Op ~= 1 and " " or "")
			joinStatementsSafe(out, formatExpr(expr.Rhs), nil)

		elseif expr.AstType == 'DotsExpr' then
			out[#out + 1] = "..."

		elseif expr.AstType == 'CallExpr' then
			out[#out + 1] = formatExpr(expr.Base)
			out[#out + 1] = "("
			for i = 1, #expr.Arguments do
				out[#out + 1] = formatExpr(expr.Arguments[i])
				if i ~= #expr.Arguments then
					out[#out + 1] = ", "
				end
			end
			out[#out + 1] = ")"

		elseif expr.AstType == 'TableCallExpr' then
			out[#out + 1] = formatExpr(expr.Base)
			out[#out + 1] = " "
			out[#out + 1] = formatExpr(expr.Arguments[1])

		elseif expr.AstType == 'StringCallExpr' then
			out[#out + 1] = formatExpr(expr.Base)
			out[#out + 1] = " "
			out[#out + 1] = expr.Arguments[1].Data

		elseif expr.AstType == 'IndexExpr' then
			out[#out + 1] = formatExpr(expr.Base)
			out[#out + 1] = "["
			out[#out + 1] = formatExpr(expr.Index)
			out[#out + 1] = "]"

		elseif expr.AstType == 'MemberExpr' then
			out[#out + 1] = formatExpr(expr.Base)
			local targetLine = lineMap[expr.Ident.Line]
			if targetLine and currentLine < targetLine then
				out[#out + 1] = string.rep('\n', targetLine - currentLine)
				currentLine = targetLine
			end
			out[#out + 1] = expr.Indexer
			out[#out + 1] = expr.Ident.Data

		elseif expr.AstType == 'Function' then
			-- anonymous function
			out[#out + 1] = "function("
			if #expr.Arguments > 0 then
				for i = 1, #expr.Arguments do
					out[#out + 1] = expr.Arguments[i].Name
					if i ~= #expr.Arguments then
						out[#out + 1] = ", "
					elseif expr.VarArg then
						out[#out + 1] = ", ..."
					end
				end
			elseif expr.VarArg then
				out[#out + 1] = "..."
			end
			out[#out + 1] = ")"
			joinStatementsSafe(out, formatStatlist(expr.Body), nil)
			joinStatementsSafe(out, "end", nil)
		elseif expr.AstType == 'ConstructorExpr' then
			out[#out + 1] = "{ "
			for i = 1, #expr.EntryList do
				local entry = expr.EntryList[i]
				if entry.Type == 'Key' then
					out[#out + 1] = "["
					out[#out + 1] = formatExpr(entry.Key)
					out[#out + 1] = "] = "
					out[#out + 1] = formatExpr(entry.Value)
				elseif entry.Type == 'Value' then
					out[#out + 1] = formatExpr(entry.Value)
				elseif entry.Type == 'KeyString' then
					out[#out + 1] = entry.Key
					out[#out + 1] = " = "
					out[#out + 1] = formatExpr(entry.Value)
				end
				if i ~= #expr.EntryList then
					out[#out + 1] = ", "
				end
			end
			out[#out + 1] = " }"

		elseif expr.AstType == 'Parentheses' then
			out[#out + 1] = "("
			out[#out + 1] = formatExpr(expr.Inner)
			out[#out + 1] = ")"

		end
		out[#out + 1] = string.rep(')', expr.ParenCount or 0)
		if expr.Tokens and expr.Tokens[1] then
			local line = expr.Tokens[1].Line
			local targetLine = lineMap[line]
			if targetLine and currentLine < targetLine then
				table.insert(out, 1, string.rep('\n', targetLine - currentLine))
				currentLine = targetLine
			end
		end
		return table.concat(out)
	end

	local formatStatement = function(statement)
		local out = {""}
		if statement.AstType == 'AssignmentStatement' then
			for i = 1, #statement.Lhs do
				out[#out + 1] = formatExpr(statement.Lhs[i])
				if i ~= #statement.Lhs then
					out[#out + 1] = ", "
				end
			end
			if #statement.Rhs > 0 then
				out[#out + 1] = " = "
				for i = 1, #statement.Rhs do
					out[#out + 1] = formatExpr(statement.Rhs[i])
					if i ~= #statement.Rhs then
						out[#out + 1] = ", "
					end
				end
			end
		elseif statement.AstType == 'CallStatement' then
			out[#out + 1] = formatExpr(statement.Expression)
		elseif statement.AstType == 'LocalStatement' then
			out[#out + 1] = "local "
			for i = 1, #statement.LocalList do
				out[#out + 1] = statement.LocalList[i].Name
				if statement.AttrList[i] then
					out[#out + 1] = " <"
					out[#out + 1] = statement.AttrList[i]
					out[#out + 1] = ">"
				end
				if i ~= #statement.LocalList then
					out[#out + 1] = ","
				end
			end
			if #statement.InitList > 0 then
				out[#out + 1] = " = "
				for i = 1, #statement.InitList do
					out[#out + 1] = formatExpr(statement.InitList[i])
					if i ~= #statement.InitList then
						out[#out + 1] = ", "
					end
				end
			end
		elseif statement.AstType == 'IfStatement' then
			out[#out + 1] = "if "
			joinStatementsSafe(out, formatExpr(statement.Clauses[1].Condition), nil)
			joinStatementsSafe(out, " then", nil)
			joinStatementsSafe(out, formatStatlist(statement.Clauses[1].Body), nil)
			for i = 2, #statement.Clauses do
				local st = statement.Clauses[i]
				if st.Condition then
					joinStatementsSafe(out, "elseif ", nil)
					joinStatementsSafe(out, formatExpr(st.Condition), nil)
					joinStatementsSafe(out, " then", nil)
				else
					joinStatementsSafe(out, "else", nil)
				end
				joinStatementsSafe(out, formatStatlist(st.Body), nil)
			end
			joinStatementsSafe(out, "end", nil)
		elseif statement.AstType == 'WhileStatement' then
			out[#out + 1] = "while "
			joinStatementsSafe(out, formatExpr(statement.Condition), nil)
			joinStatementsSafe(out, " do", nil)
			joinStatementsSafe(out, formatStatlist(statement.Body), nil)
			joinStatementsSafe(out, "end", nil)
		elseif statement.AstType == 'DoStatement' then
			joinStatementsSafe(out, "do", nil)
			joinStatementsSafe(out, formatStatlist(statement.Body), nil)
			joinStatementsSafe(out, "end", nil)
		elseif statement.AstType == 'ReturnStatement' then
			out[#out + 1] = "return "
			for i = 1, #statement.Arguments do
				joinStatementsSafe(out, formatExpr(statement.Arguments[i]), nil)
				if i ~= #statement.Arguments then
					out[#out + 1] = ", "
				end
			end
		elseif statement.AstType == 'BreakStatement' then
			out[#out + 1] = "break"
		elseif statement.AstType == 'RepeatStatement' then
			out[#out + 1] = "repeat"
			joinStatementsSafe(out, formatStatlist(statement.Body), nil)
			joinStatementsSafe(out, "until ", nil)
			joinStatementsSafe(out, formatExpr(statement.Condition), nil)
		elseif statement.AstType == 'Function' then
			if statement.IsLocal then
				out[#out + 1] = "local "
			end
			joinStatementsSafe(out, "function ", nil)
			if statement.IsLocal then
				out[#out + 1] = statement.Name.Name
			else
				out[#out + 1] = formatExpr(statement.Name)
			end
			out[#out + 1] = "("
			if #statement.Arguments > 0 then
				for i = 1, #statement.Arguments do
					out[#out + 1] = statement.Arguments[i].Name
					if i ~= #statement.Arguments then
						out[#out + 1] = ", "
					elseif statement.VarArg then
						out[#out + 1] = ",..."
					end
				end
			elseif statement.VarArg then
				out[#out + 1] = "..."
			end
			out[#out + 1] = ")"
			joinStatementsSafe(out, formatStatlist(statement.Body), nil)
			joinStatementsSafe(out, "end", nil)
		elseif statement.AstType == 'GenericForStatement' then
			out[#out + 1] = "for "
			for i = 1, #statement.VariableList do
				out[#out + 1] = statement.VariableList[i].Name
				if i ~= #statement.VariableList then
					out[#out + 1] = ", "
				end
			end
			out[#out + 1] = " in "
			for i = 1, #statement.Generators do
				joinStatementsSafe(out, formatExpr(statement.Generators[i]), nil)
				if i ~= #statement.Generators then
					joinStatementsSafe(out, ', ', nil)
				end
			end
			joinStatementsSafe(out, " do", nil)
			joinStatementsSafe(out, formatStatlist(statement.Body), nil)
			joinStatementsSafe(out, "end", nil)
		elseif statement.AstType == 'NumericForStatement' then
			out[#out + 1] = "for "
			out[#out + 1] = statement.Variable.Name
			out[#out + 1] = " = "
			out[#out + 1] = formatExpr(statement.Start)
			out[#out + 1] = ", "
			out[#out + 1] = formatExpr(statement.End)
			if statement.Step then
				out[#out + 1] = ", "
				out[#out + 1] = formatExpr(statement.Step)
			end
			joinStatementsSafe(out, " do", nil)
			joinStatementsSafe(out, formatStatlist(statement.Body), nil)
			joinStatementsSafe(out, "end", nil)
		elseif statement.AstType == 'LabelStatement' then
			out[#out + 1] = "::"
			out[#out + 1] = statement.Label
			out[#out + 1] = "::"
		elseif statement.AstType == 'GotoStatement' then
			out[#out + 1] = "goto "
			out[#out + 1] = statement.Label
		elseif statement.AstType == 'Comment' then
			-- Ignore
		elseif statement.AstType == 'Eof' then
			-- Ignore
		else
			print("Unknown AST Type: ", statement.AstType)
		end
		if statement.Tokens and statement.Tokens[1] then
			local line = statement.Tokens[1].Line
			local targetLine = lineMap[line]
			if targetLine and currentLine < targetLine then
				table.insert(out, 1, string.rep('\n', targetLine - currentLine))
				currentLine = targetLine
			end
		end
		return table.concat(out)
	end

	formatStatlist = function(statList)
		local out = {""}
		for _, stat in pairs(statList.Body) do
			joinStatementsSafe(out, formatStatement(stat), ';')
		end
		return table.concat(out)
	end

	return formatStatlist(ast)
end

local function GetYueLineMap(luaCodes)
	local current = 1
	local lastLine = 1
	local lineMap = { }
	for lineCode in luaCodes:gmatch("[^\n\r]*") do
		local num = lineCode:match("--%s*(%d+)%s*$")
		if num then
			local line = tonumber(num)
			if line > lastLine then
				lastLine = line
			end
		end
		lineMap[current] = lastLine
		current = current + 1
	end
	return lineMap
end

return {
	FormatMini = function(src)
		local st, ast = ParseLua(src)
		if st then
			return Format_Mini(ast)
		else
			return nil, ast
		end
	end,

	FormatYue = function(src)
		local st, ast = ParseLua(src)
		if st then
			local lineMap = GetYueLineMap(src)
			if #lineMap == 0 then
				return src
			end
			return FormatYue(ast, lineMap)
		else
			return nil, ast
		end
	end
}
)lua_codes";

