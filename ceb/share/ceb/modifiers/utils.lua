function nickInSentence(sentence, nick)
    local lowerSentence, lowerNick = string.lower(sentence), string.lower(nick);
	return (string.find(lowerSentence, "^"..lowerNick.."$") ~= nil) or
		(string.find(lowerSentence, "^"..lowerNick.."[^%w]") ~= nil) or
		(string.find(lowerSentence, "[^%a]"..lowerNick.."[^%w]") ~= nil) or
		(string.find(lowerSentence, "[^%a]"..lowerNick.."$") ~= nil);
end

-- Highlight a tell tab if not focused
function highlightTellTab(peer)
	local peer_login = tokenArgument(1);

	tab = getTab("tell", peer_login);
	if not isTabFocused(tab) then
		setTabColor(tab, "FF0000");
	end
end
