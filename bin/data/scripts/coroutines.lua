all_coroutines = {}

function startCoroutine(logic, name, ...)
	local co = coroutine.create(logic)
	all_coroutines[name] = co

	coroutine.resume(co, ...)
	print("Coroutine started: "..name)
end

function updateCoroutines(...)
	local name, co = next(all_coroutines)
	while name ~= nil do
		local ok = coroutine.resume(co, ...)
		local status = coroutine.status(co)

		if not ok or status == "dead" then
			all_coroutines[name] = nil
		end

		name, co = next(all_coroutines, name)
	end
end

function clearCoroutines()
	local running_co = coroutine.running()

	local name, co = next(all_coroutines)
	while name ~= nil do
		if co ~= running_co then
			print("Coroutine removed: "..name)
			all_coroutines[name] = nil
		else
			print("WARNING: Can't remove coroutine: "..name)
		end

		name, co = next(all_coroutines, name)
	end
end

function dumpCoroutines()
	print("---------")
	print("Dumping coroutines:")
	local name, co = next(all_coroutines)
	while name ~= nil do
		print(" "..name..": "..coroutine.status(co))

		name, co = next(all_coroutines, name)
	end
	print("---------")
end

function waitTime(duration)
	local time = 0.0
	while true do
		if time < duration then
			time = time + coroutine.yield(time)
		else
			return
		end
	end
end

function waitInput(mappedKey)
	return isKeyPressed(mappedKey)
end

function waitCondition(func, ...)
	while true do
		if func(...) then
			return
		else
			coroutine.yield()
		end
	end
end

print("Coroutine framework ready")
