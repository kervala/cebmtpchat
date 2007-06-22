-- someonesays.lua
-- By Guillaume Denry (guillaume.denry@gmail.com)
-- 1) colorize nickname in function of session login
-- 2) colorize sentence if session login is in

function NickInSentence(sentence, nick)
    local lowerSentence, lowerNick = string.lower(sentence), string.lower(nick);
	return (string.find(lowerSentence, "^"..lowerNick.."$") ~= nil) or
		(string.find(lowerSentence, "^"..lowerNick.."[^%w]") ~= nil) or
		(string.find(lowerSentence, "[^%a]"..lowerNick.."[^%w]") ~= nil) or
		(string.find(lowerSentence, "[^%a]"..lowerNick.."$") ~= nil);
end

function Modify()
	local login = get_session_info("login");

    -- Special color for nick when it's my sentence
	if login == get_segment_text(1) then
		set_segment_color(1, "000096");
	end

    -- Special color for the entire sentence when my nick is in
    if NickInSentence(get_segment_text(3), login) then
        set_segment_color(3, "FF00FF");
    end
end
