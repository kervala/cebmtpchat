-- justify.lua
-- By Benjamin "ben" Legros
-- Justify the input to 70 characters lines.

-- Copyright, 2008 Melting Pot.
--
-- This file is part of CeB.
-- CeB is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 2, or (at your option)
-- any later version.
--
-- CeB is distributed in the hope that it will be useful, but
-- WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
-- General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with CeB; see the file COPYING. If not, write to the
-- Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
-- MA 02111-1307, USA.

-- This is the function that is call just before CeB send the user text to the server
-- IN: InputString is the input string
-- RETURN: The string after the conversion
function InputFilter(InputString)

	local	output = ""

	local	maxSize = 70
	local	words = {}
	local	lineSize = 0
	for w in string.gmatch (InputString, "[^%s]+") do
		local	newSize = lineSize + #w + (#words > 0 and 1 or 0)
		if newSize > maxSize then
			local	space = math.max (1, 1 + (maxSize - lineSize) / (#words-1))
			local	ispace = math.floor (space)
			local	remainder = space - ispace
			local	accum = 0.5
			for k, w in ipairs (words) do
				if k > 1 then
					accum = accum + remainder
					output = output .. string.rep (" ", ispace + math.floor (accum))
					accum = accum - math.floor (accum)
				end
				output = output .. w
			end
			output = output .. "\n"
			words = {}
			table.insert (words, w)
			lineSize = #w
		else
			lineSize = newSize
			table.insert (words, w)
		end
	end

	for k, w in ipairs (words) do
		output = output .. (k > 1 and " " or "") .. w
	end

	return output
end
