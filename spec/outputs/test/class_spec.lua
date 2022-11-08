return describe("class", function()
	it("should make a class with constructor", function()
		local Thing
		do
			local _class_0
			local _base_0 = { }
			if _base_0.__index == nil then
				_base_0.__index = _base_0
			end
			_class_0 = setmetatable({
				__init = function(self)
					self.color = "blue"
				end,
				__base = _base_0,
				__name = "Thing"
			}, {
				__index = _base_0,
				__call = function(cls, ...)
					local _self_0 = setmetatable({ }, _base_0)
					cls.__init(_self_0, ...)
					return _self_0
				end
			})
			_base_0.__class = _class_0
			Thing = _class_0
		end
		local instance = Thing()
		return assert.same(instance, {
			color = "blue"
		})
	end)
	it("should have instance methods", function()
		local Thing
		do
			local _class_0
			local _base_0 = {
				get_color = function(self)
					return self.color
				end
			}
			if _base_0.__index == nil then
				_base_0.__index = _base_0
			end
			_class_0 = setmetatable({
				__init = function(self)
					self.color = "blue"
				end,
				__base = _base_0,
				__name = "Thing"
			}, {
				__index = _base_0,
				__call = function(cls, ...)
					local _self_0 = setmetatable({ }, _base_0)
					cls.__init(_self_0, ...)
					return _self_0
				end
			})
			_base_0.__class = _class_0
			Thing = _class_0
		end
		local instance = Thing()
		return assert.same(instance:get_color(), "blue")
	end)
	it("should have base properies from class", function()
		local Thing
		do
			local _class_0
			local _base_0 = {
				color = "blue",
				get_color = function(self)
					return self.color
				end
			}
			if _base_0.__index == nil then
				_base_0.__index = _base_0
			end
			_class_0 = setmetatable({
				__init = function() end,
				__base = _base_0,
				__name = "Thing"
			}, {
				__index = _base_0,
				__call = function(cls, ...)
					local _self_0 = setmetatable({ }, _base_0)
					cls.__init(_self_0, ...)
					return _self_0
				end
			})
			_base_0.__class = _class_0
			Thing = _class_0
		end
		local instance = Thing()
		assert.same(instance:get_color(), "blue")
		return assert.same(Thing.color, "blue")
	end)
	it("should inherit another class", function()
		local Base
		do
			local _class_0
			local _base_0 = {
				get_property = function(self)
					return self[self.property]
				end
			}
			if _base_0.__index == nil then
				_base_0.__index = _base_0
			end
			_class_0 = setmetatable({
				__init = function(self, property)
					self.property = property
				end,
				__base = _base_0,
				__name = "Base"
			}, {
				__index = _base_0,
				__call = function(cls, ...)
					local _self_0 = setmetatable({ }, _base_0)
					cls.__init(_self_0, ...)
					return _self_0
				end
			})
			_base_0.__class = _class_0
			Base = _class_0
		end
		local Thing
		do
			local _class_0
			local _parent_0 = Base
			local _base_0 = {
				color = "green"
			}
			for _key_0, _val_0 in pairs(_parent_0.__base) do
				if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
					_base_0[_key_0] = _val_0
				end
			end
			if _base_0.__index == nil then
				_base_0.__index = _base_0
			end
			setmetatable(_base_0, _parent_0.__base)
			_class_0 = setmetatable({
				__init = function(self, ...)
					return _class_0.__parent.__init(self, ...)
				end,
				__base = _base_0,
				__name = "Thing",
				__parent = _parent_0
			}, {
				__index = function(cls, name)
					local val = rawget(_base_0, name)
					if val == nil then
						local parent = rawget(cls, "__parent")
						if parent then
							return parent[name]
						end
					else
						return val
					end
				end,
				__call = function(cls, ...)
					local _self_0 = setmetatable({ }, _base_0)
					cls.__init(_self_0, ...)
					return _self_0
				end
			})
			_base_0.__class = _class_0
			if _parent_0.__inherited then
				_parent_0.__inherited(_parent_0, _class_0)
			end
			Thing = _class_0
		end
		local instance = Thing("color")
		return assert.same(instance:get_property(), "green")
	end)
	it("should have class properties", function()
		local Base
		do
			local _class_0
			local _base_0 = { }
			if _base_0.__index == nil then
				_base_0.__index = _base_0
			end
			_class_0 = setmetatable({
				__init = function() end,
				__base = _base_0,
				__name = "Base"
			}, {
				__index = _base_0,
				__call = function(cls, ...)
					local _self_0 = setmetatable({ }, _base_0)
					cls.__init(_self_0, ...)
					return _self_0
				end
			})
			_base_0.__class = _class_0
			Base = _class_0
		end
		local Thing
		do
			local _class_0
			local _parent_0 = Base
			local _base_0 = { }
			for _key_0, _val_0 in pairs(_parent_0.__base) do
				if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
					_base_0[_key_0] = _val_0
				end
			end
			if _base_0.__index == nil then
				_base_0.__index = _base_0
			end
			setmetatable(_base_0, _parent_0.__base)
			_class_0 = setmetatable({
				__init = function(self, ...)
					return _class_0.__parent.__init(self, ...)
				end,
				__base = _base_0,
				__name = "Thing",
				__parent = _parent_0
			}, {
				__index = function(cls, name)
					local val = rawget(_base_0, name)
					if val == nil then
						local parent = rawget(cls, "__parent")
						if parent then
							return parent[name]
						end
					else
						return val
					end
				end,
				__call = function(cls, ...)
					local _self_0 = setmetatable({ }, _base_0)
					cls.__init(_self_0, ...)
					return _self_0
				end
			})
			_base_0.__class = _class_0
			if _parent_0.__inherited then
				_parent_0.__inherited(_parent_0, _class_0)
			end
			Thing = _class_0
		end
		local instance = Thing()
		assert.same(Base.__name, "Base")
		assert.same(Thing.__name, "Thing")
		assert.is_true(Thing.__parent == Base)
		return assert.is_true(instance.__class == Thing)
	end)
	it("should have name when assigned", function()
		local Thing
		do
			local _class_0
			local _base_0 = { }
			if _base_0.__index == nil then
				_base_0.__index = _base_0
			end
			_class_0 = setmetatable({
				__init = function() end,
				__base = _base_0,
				__name = "Thing"
			}, {
				__index = _base_0,
				__call = function(cls, ...)
					local _self_0 = setmetatable({ }, _base_0)
					cls.__init(_self_0, ...)
					return _self_0
				end
			})
			_base_0.__class = _class_0
			Thing = _class_0
		end
		return assert.same(Thing.__name, "Thing")
	end)
	it("should not expose class properties on instance", function()
		local Thing
		do
			local _class_0
			local _base_0 = { }
			if _base_0.__index == nil then
				_base_0.__index = _base_0
			end
			_class_0 = setmetatable({
				__init = function() end,
				__base = _base_0,
				__name = "Thing"
			}, {
				__index = _base_0,
				__call = function(cls, ...)
					local _self_0 = setmetatable({ }, _base_0)
					cls.__init(_self_0, ...)
					return _self_0
				end
			})
			_base_0.__class = _class_0
			local self = _class_0;
			self.height = 10
			Thing = _class_0
		end
		Thing.color = "blue"
		local instance = Thing()
		assert.same(instance.color, nil)
		return assert.same(instance.height, nil)
	end)
	it("should expose new things added to __base", function()
		local Thing
		do
			local _class_0
			local _base_0 = { }
			if _base_0.__index == nil then
				_base_0.__index = _base_0
			end
			_class_0 = setmetatable({
				__init = function() end,
				__base = _base_0,
				__name = "Thing"
			}, {
				__index = _base_0,
				__call = function(cls, ...)
					local _self_0 = setmetatable({ }, _base_0)
					cls.__init(_self_0, ...)
					return _self_0
				end
			})
			_base_0.__class = _class_0
			Thing = _class_0
		end
		local instance = Thing()
		Thing.__base.color = "green"
		return assert.same(instance.color, "green")
	end)
	it("should call with correct receiver", function()
		local instance
		local Thing
		do
			local _class_0
			local _base_0 = {
				is_class = function(self)
					return assert.is_true(self == Thing)
				end,
				is_instance = function(self)
					return assert.is_true(self == instance)
				end,
				go = function(self)
					self.__class:is_class()
					return self:is_instance()
				end
			}
			if _base_0.__index == nil then
				_base_0.__index = _base_0
			end
			_class_0 = setmetatable({
				__init = function() end,
				__base = _base_0,
				__name = "Thing"
			}, {
				__index = _base_0,
				__call = function(cls, ...)
					local _self_0 = setmetatable({ }, _base_0)
					cls.__init(_self_0, ...)
					return _self_0
				end
			})
			_base_0.__class = _class_0
			Thing = _class_0
		end
		instance = Thing()
		return instance:go()
	end)
	it("should have class properies take precedence over base properties", function()
		local Thing
		do
			local _class_0
			local _base_0 = {
				prop = "world"
			}
			if _base_0.__index == nil then
				_base_0.__index = _base_0
			end
			_class_0 = setmetatable({
				__init = function() end,
				__base = _base_0,
				__name = "Thing"
			}, {
				__index = _base_0,
				__call = function(cls, ...)
					local _self_0 = setmetatable({ }, _base_0)
					cls.__init(_self_0, ...)
					return _self_0
				end
			})
			_base_0.__class = _class_0
			local self = _class_0;
			self.prop = "hello"
			Thing = _class_0
		end
		return assert.same("hello", Thing.prop)
	end)
	return describe("super", function()
		it("should call super constructor", function()
			local Base
			do
				local _class_0
				local _base_0 = { }
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				_class_0 = setmetatable({
					__init = function(self, property)
						self.property = property
					end,
					__base = _base_0,
					__name = "Base"
				}, {
					__index = _base_0,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				Base = _class_0
			end
			local Thing
			do
				local _class_0
				local _parent_0 = Base
				local _base_0 = { }
				for _key_0, _val_0 in pairs(_parent_0.__base) do
					if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
						_base_0[_key_0] = _val_0
					end
				end
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				setmetatable(_base_0, _parent_0.__base)
				_class_0 = setmetatable({
					__init = function(self, name)
						self.name = name
						return _class_0.__parent.__init(self, "name")
					end,
					__base = _base_0,
					__name = "Thing",
					__parent = _parent_0
				}, {
					__index = function(cls, name)
						local val = rawget(_base_0, name)
						if val == nil then
							local parent = rawget(cls, "__parent")
							if parent then
								return parent[name]
							end
						else
							return val
						end
					end,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				if _parent_0.__inherited then
					_parent_0.__inherited(_parent_0, _class_0)
				end
				Thing = _class_0
			end
			local instance = Thing("the_thing")
			assert.same(instance.property, "name")
			return assert.same(instance.name, "the_thing")
		end)
		it("should call super method", function()
			local Base
			do
				local _class_0
				local _base_0 = {
					_count = 111,
					counter = function(self)
						return self._count
					end
				}
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				_class_0 = setmetatable({
					__init = function() end,
					__base = _base_0,
					__name = "Base"
				}, {
					__index = _base_0,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				Base = _class_0
			end
			local Thing
			do
				local _class_0
				local _parent_0 = Base
				local _base_0 = {
					counter = function(self)
						return ("%08d"):format(_class_0.__parent.__base.counter(self))
					end
				}
				for _key_0, _val_0 in pairs(_parent_0.__base) do
					if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
						_base_0[_key_0] = _val_0
					end
				end
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				setmetatable(_base_0, _parent_0.__base)
				_class_0 = setmetatable({
					__init = function(self, ...)
						return _class_0.__parent.__init(self, ...)
					end,
					__base = _base_0,
					__name = "Thing",
					__parent = _parent_0
				}, {
					__index = function(cls, name)
						local val = rawget(_base_0, name)
						if val == nil then
							local parent = rawget(cls, "__parent")
							if parent then
								return parent[name]
							end
						else
							return val
						end
					end,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				if _parent_0.__inherited then
					_parent_0.__inherited(_parent_0, _class_0)
				end
				Thing = _class_0
			end
			local instance = Thing()
			return assert.same(instance:counter(), "00000111")
		end)
		it("should call other method from super", function()
			local Base
			do
				local _class_0
				local _base_0 = {
					_count = 111,
					counter = function(self)
						return self._count
					end
				}
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				_class_0 = setmetatable({
					__init = function() end,
					__base = _base_0,
					__name = "Base"
				}, {
					__index = _base_0,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				Base = _class_0
			end
			local Thing
			do
				local _class_0
				local _parent_0 = Base
				local _base_0 = {
					other_method = function(self)
						return _class_0.__parent.counter(self)
					end
				}
				for _key_0, _val_0 in pairs(_parent_0.__base) do
					if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
						_base_0[_key_0] = _val_0
					end
				end
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				setmetatable(_base_0, _parent_0.__base)
				_class_0 = setmetatable({
					__init = function(self, ...)
						return _class_0.__parent.__init(self, ...)
					end,
					__base = _base_0,
					__name = "Thing",
					__parent = _parent_0
				}, {
					__index = function(cls, name)
						local val = rawget(_base_0, name)
						if val == nil then
							local parent = rawget(cls, "__parent")
							if parent then
								return parent[name]
							end
						else
							return val
						end
					end,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				if _parent_0.__inherited then
					_parent_0.__inherited(_parent_0, _class_0)
				end
				Thing = _class_0
			end
			local instance = Thing()
			return assert.same(instance:other_method(), 111)
		end)
		it("should get super class", function()
			local Base
			do
				local _class_0
				local _base_0 = { }
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				_class_0 = setmetatable({
					__init = function() end,
					__base = _base_0,
					__name = "Base"
				}, {
					__index = _base_0,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				Base = _class_0
			end
			local Thing
			do
				local _class_0
				local _parent_0 = Base
				local _base_0 = {
					get_super = function(self)
						return _class_0.__parent
					end
				}
				for _key_0, _val_0 in pairs(_parent_0.__base) do
					if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
						_base_0[_key_0] = _val_0
					end
				end
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				setmetatable(_base_0, _parent_0.__base)
				_class_0 = setmetatable({
					__init = function(self, ...)
						return _class_0.__parent.__init(self, ...)
					end,
					__base = _base_0,
					__name = "Thing",
					__parent = _parent_0
				}, {
					__index = function(cls, name)
						local val = rawget(_base_0, name)
						if val == nil then
							local parent = rawget(cls, "__parent")
							if parent then
								return parent[name]
							end
						else
							return val
						end
					end,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				if _parent_0.__inherited then
					_parent_0.__inherited(_parent_0, _class_0)
				end
				Thing = _class_0
			end
			local instance = Thing()
			return assert.is_true(instance:get_super() == Base)
		end)
		it("should get a bound method from super", function()
			local Base
			do
				local _class_0
				local _base_0 = {
					count = 1,
					get_count = function(self)
						return self.count
					end
				}
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				_class_0 = setmetatable({
					__init = function() end,
					__base = _base_0,
					__name = "Base"
				}, {
					__index = _base_0,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				Base = _class_0
			end
			local Thing
			do
				local _class_0
				local _parent_0 = Base
				local _base_0 = {
					get_count = function(self)
						return "this is wrong"
					end,
					get_method = function(self)
						local _base_1 = _class_0.__parent
						local _fn_0 = _base_1.get_count
						return _fn_0 and function(...)
							return _fn_0(_base_1, ...)
						end
					end
				}
				for _key_0, _val_0 in pairs(_parent_0.__base) do
					if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
						_base_0[_key_0] = _val_0
					end
				end
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				setmetatable(_base_0, _parent_0.__base)
				_class_0 = setmetatable({
					__init = function(self, ...)
						return _class_0.__parent.__init(self, ...)
					end,
					__base = _base_0,
					__name = "Thing",
					__parent = _parent_0
				}, {
					__index = function(cls, name)
						local val = rawget(_base_0, name)
						if val == nil then
							local parent = rawget(cls, "__parent")
							if parent then
								return parent[name]
							end
						else
							return val
						end
					end,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				if _parent_0.__inherited then
					_parent_0.__inherited(_parent_0, _class_0)
				end
				Thing = _class_0
			end
			local instance = Thing()
			return assert.same(instance:get_method()(), 1)
		end)
		it("class properties take precedence in super class over base", function()
			local Thing
			do
				local _class_0
				local _base_0 = {
					prop = "world"
				}
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				_class_0 = setmetatable({
					__init = function() end,
					__base = _base_0,
					__name = "Thing"
				}, {
					__index = _base_0,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				local self = _class_0;
				self.prop = "hello"
				Thing = _class_0
			end
			local OtherThing
			do
				local _class_0
				local _parent_0 = Thing
				local _base_0 = { }
				for _key_0, _val_0 in pairs(_parent_0.__base) do
					if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
						_base_0[_key_0] = _val_0
					end
				end
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				setmetatable(_base_0, _parent_0.__base)
				_class_0 = setmetatable({
					__init = function(self, ...)
						return _class_0.__parent.__init(self, ...)
					end,
					__base = _base_0,
					__name = "OtherThing",
					__parent = _parent_0
				}, {
					__index = function(cls, name)
						local val = rawget(_base_0, name)
						if val == nil then
							local parent = rawget(cls, "__parent")
							if parent then
								return parent[name]
							end
						else
							return val
						end
					end,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				if _parent_0.__inherited then
					_parent_0.__inherited(_parent_0, _class_0)
				end
				OtherThing = _class_0
			end
			return assert.same("hello", OtherThing.prop)
		end)
		it("gets value from base in super class", function()
			local Thing
			do
				local _class_0
				local _base_0 = {
					prop = "world"
				}
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				_class_0 = setmetatable({
					__init = function() end,
					__base = _base_0,
					__name = "Thing"
				}, {
					__index = _base_0,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				Thing = _class_0
			end
			local OtherThing
			do
				local _class_0
				local _parent_0 = Thing
				local _base_0 = { }
				for _key_0, _val_0 in pairs(_parent_0.__base) do
					if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
						_base_0[_key_0] = _val_0
					end
				end
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				setmetatable(_base_0, _parent_0.__base)
				_class_0 = setmetatable({
					__init = function(self, ...)
						return _class_0.__parent.__init(self, ...)
					end,
					__base = _base_0,
					__name = "OtherThing",
					__parent = _parent_0
				}, {
					__index = function(cls, name)
						local val = rawget(_base_0, name)
						if val == nil then
							local parent = rawget(cls, "__parent")
							if parent then
								return parent[name]
							end
						else
							return val
						end
					end,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				if _parent_0.__inherited then
					_parent_0.__inherited(_parent_0, _class_0)
				end
				OtherThing = _class_0
			end
			return assert.same("world", OtherThing.prop)
		end)
		it("should let parent be replaced on class", function()
			local A
			do
				local _class_0
				local _base_0 = {
					cool = function(self)
						return 1234
					end,
					plain = function(self)
						return "a"
					end
				}
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				_class_0 = setmetatable({
					__init = function() end,
					__base = _base_0,
					__name = "A"
				}, {
					__index = _base_0,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				local self = _class_0;
				self.prop = "yeah"
				A = _class_0
			end
			local B
			do
				local _class_0
				local _base_0 = {
					cool = function(self)
						return 9999
					end,
					plain = function(self)
						return "b"
					end
				}
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				_class_0 = setmetatable({
					__init = function() end,
					__base = _base_0,
					__name = "B"
				}, {
					__index = _base_0,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				local self = _class_0;
				self.prop = "okay"
				B = _class_0
			end
			local Thing
			do
				local _class_0
				local _parent_0 = A
				local _base_0 = {
					cool = function(self)
						return _class_0.__parent.__base.cool(self) + 1
					end,
					get_super = function(self)
						return _class_0.__parent
					end
				}
				for _key_0, _val_0 in pairs(_parent_0.__base) do
					if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
						_base_0[_key_0] = _val_0
					end
				end
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				setmetatable(_base_0, _parent_0.__base)
				_class_0 = setmetatable({
					__init = function(self, ...)
						return _class_0.__parent.__init(self, ...)
					end,
					__base = _base_0,
					__name = "Thing",
					__parent = _parent_0
				}, {
					__index = function(cls, name)
						local val = rawget(_base_0, name)
						if val == nil then
							local parent = rawget(cls, "__parent")
							if parent then
								return parent[name]
							end
						else
							return val
						end
					end,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				if _parent_0.__inherited then
					_parent_0.__inherited(_parent_0, _class_0)
				end
				Thing = _class_0
			end
			local instance = Thing()
			assert.same("a", instance:plain())
			assert.same(1235, instance:cool())
			assert(A == instance:get_super(), "expected super to be B")
			Thing.__parent = B
			setmetatable(Thing.__base, B.__base)
			assert.same("b", instance:plain())
			assert.same(10000, instance:cool())
			return assert(B == instance:get_super(), "expected super to be B")
		end)
		it("should resolve many levels of super", function()
			local One
			do
				local _class_0
				local _base_0 = {
					a = function(self)
						return 1
					end
				}
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				_class_0 = setmetatable({
					__init = function() end,
					__base = _base_0,
					__name = "One"
				}, {
					__index = _base_0,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				One = _class_0
			end
			local Two
			do
				local _class_0
				local _parent_0 = One
				local _base_0 = {
					a = function(self)
						return _class_0.__parent.__base.a(self) + 2
					end
				}
				for _key_0, _val_0 in pairs(_parent_0.__base) do
					if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
						_base_0[_key_0] = _val_0
					end
				end
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				setmetatable(_base_0, _parent_0.__base)
				_class_0 = setmetatable({
					__init = function(self, ...)
						return _class_0.__parent.__init(self, ...)
					end,
					__base = _base_0,
					__name = "Two",
					__parent = _parent_0
				}, {
					__index = function(cls, name)
						local val = rawget(_base_0, name)
						if val == nil then
							local parent = rawget(cls, "__parent")
							if parent then
								return parent[name]
							end
						else
							return val
						end
					end,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				if _parent_0.__inherited then
					_parent_0.__inherited(_parent_0, _class_0)
				end
				Two = _class_0
			end
			local Three
			do
				local _class_0
				local _parent_0 = Two
				local _base_0 = {
					a = function(self)
						return _class_0.__parent.__base.a(self) + 3
					end
				}
				for _key_0, _val_0 in pairs(_parent_0.__base) do
					if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
						_base_0[_key_0] = _val_0
					end
				end
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				setmetatable(_base_0, _parent_0.__base)
				_class_0 = setmetatable({
					__init = function(self, ...)
						return _class_0.__parent.__init(self, ...)
					end,
					__base = _base_0,
					__name = "Three",
					__parent = _parent_0
				}, {
					__index = function(cls, name)
						local val = rawget(_base_0, name)
						if val == nil then
							local parent = rawget(cls, "__parent")
							if parent then
								return parent[name]
							end
						else
							return val
						end
					end,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				if _parent_0.__inherited then
					_parent_0.__inherited(_parent_0, _class_0)
				end
				Three = _class_0
			end
			local i = Three()
			return assert.same(6, i:a())
		end)
		it("should resolve many levels of super with a gap", function()
			local One
			do
				local _class_0
				local _base_0 = {
					a = function(self)
						return 1
					end
				}
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				_class_0 = setmetatable({
					__init = function() end,
					__base = _base_0,
					__name = "One"
				}, {
					__index = _base_0,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				One = _class_0
			end
			local Two
			do
				local _class_0
				local _parent_0 = One
				local _base_0 = { }
				for _key_0, _val_0 in pairs(_parent_0.__base) do
					if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
						_base_0[_key_0] = _val_0
					end
				end
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				setmetatable(_base_0, _parent_0.__base)
				_class_0 = setmetatable({
					__init = function(self, ...)
						return _class_0.__parent.__init(self, ...)
					end,
					__base = _base_0,
					__name = "Two",
					__parent = _parent_0
				}, {
					__index = function(cls, name)
						local val = rawget(_base_0, name)
						if val == nil then
							local parent = rawget(cls, "__parent")
							if parent then
								return parent[name]
							end
						else
							return val
						end
					end,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				if _parent_0.__inherited then
					_parent_0.__inherited(_parent_0, _class_0)
				end
				Two = _class_0
			end
			local Three
			do
				local _class_0
				local _parent_0 = Two
				local _base_0 = {
					a = function(self)
						return _class_0.__parent.__base.a(self) + 3
					end
				}
				for _key_0, _val_0 in pairs(_parent_0.__base) do
					if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
						_base_0[_key_0] = _val_0
					end
				end
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				setmetatable(_base_0, _parent_0.__base)
				_class_0 = setmetatable({
					__init = function(self, ...)
						return _class_0.__parent.__init(self, ...)
					end,
					__base = _base_0,
					__name = "Three",
					__parent = _parent_0
				}, {
					__index = function(cls, name)
						local val = rawget(_base_0, name)
						if val == nil then
							local parent = rawget(cls, "__parent")
							if parent then
								return parent[name]
							end
						else
							return val
						end
					end,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				if _parent_0.__inherited then
					_parent_0.__inherited(_parent_0, _class_0)
				end
				Three = _class_0
			end
			local Four
			do
				local _class_0
				local _parent_0 = Three
				local _base_0 = {
					a = function(self)
						return _class_0.__parent.__base.a(self) + 4
					end
				}
				for _key_0, _val_0 in pairs(_parent_0.__base) do
					if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
						_base_0[_key_0] = _val_0
					end
				end
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				setmetatable(_base_0, _parent_0.__base)
				_class_0 = setmetatable({
					__init = function(self, ...)
						return _class_0.__parent.__init(self, ...)
					end,
					__base = _base_0,
					__name = "Four",
					__parent = _parent_0
				}, {
					__index = function(cls, name)
						local val = rawget(_base_0, name)
						if val == nil then
							local parent = rawget(cls, "__parent")
							if parent then
								return parent[name]
							end
						else
							return val
						end
					end,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				if _parent_0.__inherited then
					_parent_0.__inherited(_parent_0, _class_0)
				end
				Four = _class_0
			end
			local i = Four()
			return assert.same(8, i:a())
		end)
		it("should call correct class/instance super methods", function()
			local Base
			do
				local _class_0
				local _base_0 = {
					doit = function(self)
						return "instance"
					end
				}
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				_class_0 = setmetatable({
					__init = function() end,
					__base = _base_0,
					__name = "Base"
				}, {
					__index = _base_0,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				local self = _class_0;
				self.doit = function(self)
					return "class"
				end
				Base = _class_0
			end
			local One
			do
				local _class_0
				local _parent_0 = Base
				local _base_0 = {
					doit = function(self)
						return _class_0.__parent.__base.doit(self)
					end
				}
				for _key_0, _val_0 in pairs(_parent_0.__base) do
					if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
						_base_0[_key_0] = _val_0
					end
				end
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				setmetatable(_base_0, _parent_0.__base)
				_class_0 = setmetatable({
					__init = function(self, ...)
						return _class_0.__parent.__init(self, ...)
					end,
					__base = _base_0,
					__name = "One",
					__parent = _parent_0
				}, {
					__index = function(cls, name)
						local val = rawget(_base_0, name)
						if val == nil then
							local parent = rawget(cls, "__parent")
							if parent then
								return parent[name]
							end
						else
							return val
						end
					end,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				local self = _class_0;
				self.doit = function(self)
					return _class_0.__parent.doit(self)
				end
				if _parent_0.__inherited then
					_parent_0.__inherited(_parent_0, _class_0)
				end
				One = _class_0
			end
			assert.same("instance", One():doit())
			return assert.same("class", One:doit())
		end)
		it("should resolve many levels of super on class methods", function()
			local One
			do
				local _class_0
				local _base_0 = { }
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				_class_0 = setmetatable({
					__init = function() end,
					__base = _base_0,
					__name = "One"
				}, {
					__index = _base_0,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				local self = _class_0;
				self.a = function(self)
					return 1
				end
				One = _class_0
			end
			local Two
			do
				local _class_0
				local _parent_0 = One
				local _base_0 = { }
				for _key_0, _val_0 in pairs(_parent_0.__base) do
					if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
						_base_0[_key_0] = _val_0
					end
				end
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				setmetatable(_base_0, _parent_0.__base)
				_class_0 = setmetatable({
					__init = function(self, ...)
						return _class_0.__parent.__init(self, ...)
					end,
					__base = _base_0,
					__name = "Two",
					__parent = _parent_0
				}, {
					__index = function(cls, name)
						local val = rawget(_base_0, name)
						if val == nil then
							local parent = rawget(cls, "__parent")
							if parent then
								return parent[name]
							end
						else
							return val
						end
					end,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				if _parent_0.__inherited then
					_parent_0.__inherited(_parent_0, _class_0)
				end
				Two = _class_0
			end
			local Three
			do
				local _class_0
				local _parent_0 = Two
				local _base_0 = { }
				for _key_0, _val_0 in pairs(_parent_0.__base) do
					if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
						_base_0[_key_0] = _val_0
					end
				end
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				setmetatable(_base_0, _parent_0.__base)
				_class_0 = setmetatable({
					__init = function(self, ...)
						return _class_0.__parent.__init(self, ...)
					end,
					__base = _base_0,
					__name = "Three",
					__parent = _parent_0
				}, {
					__index = function(cls, name)
						local val = rawget(_base_0, name)
						if val == nil then
							local parent = rawget(cls, "__parent")
							if parent then
								return parent[name]
							end
						else
							return val
						end
					end,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				local self = _class_0;
				self.a = function(self)
					return _class_0.__parent.a(self) + 3
				end
				if _parent_0.__inherited then
					_parent_0.__inherited(_parent_0, _class_0)
				end
				Three = _class_0
			end
			local Four
			do
				local _class_0
				local _parent_0 = Three
				local _base_0 = { }
				for _key_0, _val_0 in pairs(_parent_0.__base) do
					if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
						_base_0[_key_0] = _val_0
					end
				end
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				setmetatable(_base_0, _parent_0.__base)
				_class_0 = setmetatable({
					__init = function(self, ...)
						return _class_0.__parent.__init(self, ...)
					end,
					__base = _base_0,
					__name = "Four",
					__parent = _parent_0
				}, {
					__index = function(cls, name)
						local val = rawget(_base_0, name)
						if val == nil then
							local parent = rawget(cls, "__parent")
							if parent then
								return parent[name]
							end
						else
							return val
						end
					end,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				local self = _class_0;
				self.a = function(self)
					return _class_0.__parent.a(self) + 4
				end
				if _parent_0.__inherited then
					_parent_0.__inherited(_parent_0, _class_0)
				end
				Four = _class_0
			end
			return assert.same(8, Four:a())
		end)
		it("super should still work when method wrapped", function()
			local add_some
			add_some = function(opts)
				return function(self)
					return opts.amount + opts[1](self)
				end
			end
			local Base
			do
				local _class_0
				local _base_0 = {
					value = function(self)
						return 1
					end
				}
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				_class_0 = setmetatable({
					__init = function() end,
					__base = _base_0,
					__name = "Base"
				}, {
					__index = _base_0,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				Base = _class_0
			end
			local Sub
			do
				local _class_0
				local _parent_0 = Base
				local _base_0 = {
					value = add_some({
						amount = 12,
						function(self)
							return _class_0.__parent.__base.value(self) + 100
						end
					})
				}
				for _key_0, _val_0 in pairs(_parent_0.__base) do
					if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
						_base_0[_key_0] = _val_0
					end
				end
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				setmetatable(_base_0, _parent_0.__base)
				_class_0 = setmetatable({
					__init = function(self, ...)
						return _class_0.__parent.__init(self, ...)
					end,
					__base = _base_0,
					__name = "Sub",
					__parent = _parent_0
				}, {
					__index = function(cls, name)
						local val = rawget(_base_0, name)
						if val == nil then
							local parent = rawget(cls, "__parent")
							if parent then
								return parent[name]
							end
						else
							return val
						end
					end,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				if _parent_0.__inherited then
					_parent_0.__inherited(_parent_0, _class_0)
				end
				Sub = _class_0
			end
			local OtherSub
			do
				local _class_0
				local _parent_0 = Base
				local _base_0 = {
					value = (function()
						if true then
							return function(self)
								return 5 + _class_0.__parent.__base.value(self)
							end
						else
							return function(self)
								return 2 + _class_0.__parent.__base.value(self)
							end
						end
					end)()
				}
				for _key_0, _val_0 in pairs(_parent_0.__base) do
					if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
						_base_0[_key_0] = _val_0
					end
				end
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				setmetatable(_base_0, _parent_0.__base)
				_class_0 = setmetatable({
					__init = function(self, ...)
						return _class_0.__parent.__init(self, ...)
					end,
					__base = _base_0,
					__name = "OtherSub",
					__parent = _parent_0
				}, {
					__index = function(cls, name)
						local val = rawget(_base_0, name)
						if val == nil then
							local parent = rawget(cls, "__parent")
							if parent then
								return parent[name]
							end
						else
							return val
						end
					end,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				if _parent_0.__inherited then
					_parent_0.__inherited(_parent_0, _class_0)
				end
				OtherSub = _class_0
			end
			assert.same(1 + 100 + 12, Sub():value())
			return assert.same(6, OtherSub():value())
		end)
		return it("should copy metamethod from super", function()
			local A
			do
				local _class_0
				local _base_0 = {
					val = 1,
					__tostring = function(self)
						return "Object " .. tostring(self.val)
					end
				}
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				_class_0 = setmetatable({
					__init = function() end,
					__base = _base_0,
					__name = "A"
				}, {
					__index = _base_0,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				A = _class_0
			end
			local B
			do
				local _class_0
				local _parent_0 = A
				local _base_0 = {
					val = 2
				}
				for _key_0, _val_0 in pairs(_parent_0.__base) do
					if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
						_base_0[_key_0] = _val_0
					end
				end
				if _base_0.__index == nil then
					_base_0.__index = _base_0
				end
				setmetatable(_base_0, _parent_0.__base)
				_class_0 = setmetatable({
					__init = function(self, ...)
						return _class_0.__parent.__init(self, ...)
					end,
					__base = _base_0,
					__name = "B",
					__parent = _parent_0
				}, {
					__index = function(cls, name)
						local val = rawget(_base_0, name)
						if val == nil then
							local parent = rawget(cls, "__parent")
							if parent then
								return parent[name]
							end
						else
							return val
						end
					end,
					__call = function(cls, ...)
						local _self_0 = setmetatable({ }, _base_0)
						cls.__init(_self_0, ...)
						return _self_0
					end
				})
				_base_0.__class = _class_0
				if _parent_0.__inherited then
					_parent_0.__inherited(_parent_0, _class_0)
				end
				B = _class_0
			end
			local a, b = A(), B()
			assert.same("Object 1", tostring(a))
			return assert.same("Object 2", tostring(b))
		end)
	end)
end)
