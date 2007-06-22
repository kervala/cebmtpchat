-- someonesays.lua
-- By Guillaume Denry (guillaume.denry@gmail.com)
-- 1) colorize nickname in function of session login
-- 2) colorize sentence if session login is in

function NickInSentence(sentence, nick)
	return (string.find(sentence, "^"..nick.."$") ~= nil) or
		(string.find(sentence, "^"..nick.."[^%w]") ~= nil) or
		(string.find(sentence, "[^%a]"..nick.."[^%w]") ~= nil) or
		(string.find(sentence, "[^%a]"..nick.."$") ~= nil);
end

function Modify()
	login = get_session_info("login");
	someone = get_segment_text(1);
	if login == someone then
		set_segment_color(1, "000096");
	end

	lowerSentence = string.lower(get_segment_text(3));
	lowerNick = string.lower(login);

    if NickInSentence(lowerSentence, lowerNick) then
        set_segment_color(3, "FF00FF");
    end
end
