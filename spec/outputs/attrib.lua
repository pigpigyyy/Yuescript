do
	local a <close>, b <close> = setmetatable({ }, {
		__close = function(self)
			return print("closed")
		end
	})
	local c <const>, d <const> = 123, 'abc'
	close(a, b)
	const(c, d)
end
do
	local v <close> = (function()
		if flag then
			return func()
		else
			return setmetatable({ }, {
				__close = function(self) end
			})
		end
	end)()
	local f <close> = (function()
		local _with_0 = io.open("file.txt")
		_with_0:write("Hello")
		return _with_0
	end)()
end
do
	local _ <close> = (function()
		local _with_0 = io.open("file.txt")
		_with_0:write("Hello")
		return _with_0
	end)();
	local _ <close> = setmetatable({ }, {
		__close = function(self)
			return print("second")
		end
	});
	local _ <close> = setmetatable({ }, {
		__close = function()
			return print("first")
		end
	})
end
local _defers = setmetatable({ }, {
	__close = function(self)
		self[#self]()
		self[#self] = nil
	end
})
do
	_defers[#_defers + 1] = function()
		return print(3)
	end
	local _ <close> = _defers;
	_defers[#_defers + 1] = function()
		return print(2)
	end
	local _ <close> = _defers;
	_defers[#_defers + 1] = function()
		return print(1)
	end
	local _ <close> = _defers
end
