-- event.lua
-- By Kervala
-- Colorize user background in function of their group

function userBackgroundColor(login)

	groups =
	{
		["HeadArch"] = "#ffff00",
		["SnrGM"] = "#660066",
		["EvtMngr"] = "#00ffff",
		["GM"] = "#ff0000",
		["VtnGuide"] = "#00ff00",
		["SnrGuide"] = "#0000ff",
		["EvtGuide"] = "#0000ff",
		["Guide"] = "#ff8000"
	}

	color = groups[getUserData(login, "group")]

	if color then
		setUserBackgroundColor(getColor(color, 64))
	end
end
