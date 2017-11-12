-- someonesays.lua
-- By Guillaume Denry (guillaume.denry@gmail.com)
-- 1) colorize nickname in function of session login
-- 2) colorize sentence if session login is in

require "utils"

function render()
	local login = getSessionInfo("login")

	-- Special color for nick when it's my sentence
	if login == getSegmentText(1) then
		setSegmentColor(1, "#000096")
	end

	-- Special color for the entire sentence when my nick is in
	sentence = getSegmentText(3)
	len = string.len(login)
        t = nickInSentence(sentence, login)
	f = getSegmentFont(3)
	s = getSegmentSize(3)
	c = getSegmentColor(3)
	off = 1
	strings = {}
	startsWithNick = false
	for i = 1, # t do
		pos = t[i]
		if pos > off then
			table.insert(strings, string.sub(sentence, off, pos - 1))
		elseif i == 1 then
			startsWithNick = true
		end
		table.insert(strings, string.sub(sentence, pos, pos + len - 1))
		off = pos + len
	end
	if off <= string.len(sentence) then
		table.insert(strings, string.sub(sentence, off, string.len(sentence)))
	end

	if # strings > 1 then
		setSegmentsCount(segmentsCount() + # strings - 1)
	end

	for i = 1, # strings do
		setSegmentText(3 + i - 1, strings[i])
		setSegmentFont(3 + i - 1, f)
		setSegmentSize(3 + i - 1, s)
		if startsWithNick then
			setSegmentColor(3 + i - 1, "#FF00FF")
			startsWithNick = false
		else
			setSegmentColor(3 + i - 1, c)
			startsWithNick = true
		end
	end
end

function newToken()
	tab = getTab("channel");
	if not isTabFocused(tab) then
		c = getTabColor(tab)
		if c ~= getColor("#FF0000") then
			if # nickInSentence(tokenArgument(2), getSessionInfo("login")) > 0 then
				setTabColor(tab, "#FF0000")
			else
				setTabColor(tab, "#0000FF")
			end
		end
	end
end
