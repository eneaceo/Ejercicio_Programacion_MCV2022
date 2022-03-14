print('-----------')
--print('This is lua. How are you?')

-- create
-- yield
-- status
-- resume
-- running
-- wrap

clearCoroutines()

flag_AnnaReady = false

function AnnaIsReady()
	return flag_AnnaReady
end

function dialog(name1, name2)
	print('Hello')
	waitTime(2.0)
	clearCoroutines()
	print("My name is "..name1)
	waitCondition(AnnaIsReady)
	print("How are you doing, "..name2)
	waitTime(2.0)
	print('Bye '..name2)
end

function MovieGenerator()
	local titles = {"Terminator", "Jurassic Park", "X-Men", "Avengers"}
	local subtitles = {"The Beginning", "Apocalypse", "Origins", "Genysis", "Endworld"}

	while true do
		local name = titles[math.random(4)].." "..tostring(math.random(10))..": "..subtitles[math.random(5)]
		coroutine.yield(name)
	end
end

--local generateMovie = coroutine.wrap(MovieGenerator)

--print(generateMovie(""))
--print(generateMovie())
--print(generateMovie())
--print(generateMovie())
--print(generateMovie())
--print(generateMovie())
--print(generateMovie())
--print(generateMovie())
--print(generateMovie())

--local co = coroutine.create(MovieGenerator)

--local ok, movieName = coroutine.resume(co)
--print("Movie: "..movieName)


function OnEnterMainLobby()
	chCharlie = getCharacter("charlie")
	getAnimation("enter")
	playAnimation("Charlie", "enter")
	waitTime(3.0)
	playDialog("Anna", "greetings_01")
	waitInput("continue")
end

--local dialogCo = coroutine.create(dialog)
--coroutine.resume(dialogCo)

startCoroutine(dialog, "dialog", "Charlie", "Anna")
