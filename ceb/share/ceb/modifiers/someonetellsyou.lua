-- someonetellsyou.lua
-- By Guillaume Denry (guillaume.denry@gmail.com)
-- Colorizes the good tell tab

function newToken()
	local peer_login = tokenArgument(1);

	tab = getTab("tell", peer_login);
	if not isTabFocused(tab) then
		setTabColor(tab, "FF0000");
	end
end
