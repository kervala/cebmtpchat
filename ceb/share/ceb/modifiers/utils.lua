function nickInSentence(sentence, nick)
    local lowerSentence, lowerNick = string.lower(sentence), string.lower(nick);
	return (string.find(lowerSentence, "^"..lowerNick.."$") ~= nil) or
		(string.find(lowerSentence, "^"..lowerNick.."[^%w]") ~= nil) or
		(string.find(lowerSentence, "[^%a]"..lowerNick.."[^%w]") ~= nil) or
		(string.find(lowerSentence, "[^%a]"..lowerNick.."$") ~= nil);
end
