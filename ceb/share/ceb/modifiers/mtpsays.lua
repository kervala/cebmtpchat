-- someonesays.lua
-- By Guillaume Denry (guillaume.denry@gmail.com)
-- 1) colorize nickname in function of session login
-- 2) colorize sentence if session login is in

require "utils"

function newToken()
	tab = getTab("channel");
	if not isTabFocused(tab) then
		setTabColor(tab, "#0000FF");
	end
end
