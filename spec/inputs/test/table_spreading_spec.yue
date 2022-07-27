describe "table spreading", ->
	it "list and dict", ->
		template = {
			foo: "Hello"
			bar: "World"
			"!"
		}

		specialized = {
			"a", "b", "c"
			...template
			bar: "Bob"
		}

		assert.same specialized, {
			"a"
			"b"
			"c"
			"!"
			foo: "Hello"
			bar: "Bob"
		}
