do
	a = 0
	::start::
	a += 1
	goto done if a == 5
	goto start
	::done::

do
	for z = 1, 10 for y = 1, 10 for x = 1, 10
		if x^2 + y^2 == z^2
			print 'found a Pythagorean triple:', x, y, z
			goto done
	::done::

do
	for z = 1, 10
		for y = 1, 10 for x = 1, 10
				if x^2 + y^2 == z^2
					print 'found a Pythagorean triple:', x, y, z
					print 'now trying next z...'
					goto zcontinue
		::zcontinue::

do
	::redo::
	for x = 1, 10 for y = 1, 10
		if not f x, y then goto continue
		if not g x, y then goto skip
		if not h x, y then goto redo
		::continue::
	::skip::

do
	for x in *t
		if x % 2 == 0
			print 'list has even number'
			goto has
	print 'list lacks even number'
	::has::

