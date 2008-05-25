-- someonesays.lua
-- By Guillaume Denry (guillaume.denry@gmail.com)
-- 1) colorize nickname in function of session login
-- 2) colorize sentence if session login is in

require "utils"

function render()
	local login = getSessionInfo("login");

	-- Special color for nick when it's my sentence
	if login == getSegmentText(1) then
		setSegmentColor(1, "000096");
	end

	-- Special color for the entire sentence when my nick is in
	if nickInSentence(getSegmentText(3), login) then
		setSegmentColor(3, "FF00FF");
	end
end

function newToken()
	tab = getTab("channel");
	if not isTabFocused(tab) then
		setTabColor(tab, "0000FF");
	end
end
