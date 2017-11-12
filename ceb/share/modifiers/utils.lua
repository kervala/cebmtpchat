function wordLetterBeforePos(sentence, nick, pos)
	if pos == 1 then
		return false
	end

        if not string.find(string.sub(sentence, pos - 1, pos - 1), "%w") then
		return false
	end

	return true
end

function wordLetterAfterPos(sentence, nick, pos)
	if pos == string.len(sentence) then
		return false
	end

        if not string.find(string.sub(sentence, pos + 1, pos + 1), "%w") then
		return false
	end
	return true
end

function findNick(sentence, nick, init)
	local lowerSentence, lowerNick = string.lower(sentence), string.lower(nick)

	pos = string.find(lowerSentence, lowerNick, init)

	if (pos ~= nil) and (pos >= 1) and
          not wordLetterBeforePos(lowerSentence, lowerNick, pos) and
	  not wordLetterAfterPos(lowerSentence, lowerNick, pos + string.len(lowerNick) - 1) then
		return pos
	end
	return nil
end

function nickInSentence(sentence, nick)
        t = {}
        b = findNick(sentence, nick, 1, false)
	while b ~= nil do
		table.insert(t, b)
		b = findNick(sentence, nick, b + string.len(nick))
	end
	return t
end

-- Highlight a tell tab if not focused
function highlightTellTab(peer)
	local peer_login = tokenArgument(1)

	tab = getTab("tell", peer_login)
	if not isTabFocused(tab) then
		setTabColor(tab, "#FF0000")
	end
end
