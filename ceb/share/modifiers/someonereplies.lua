-- someonetellsyou.lua
-- By Guillaume Denry (guillaume.denry@gmail.com)
-- Colorizes the good tell tab

require "utils"

function newToken()
	highlightTellTab(tokenArgument(1));
end
