-- haxor.lua
-- By Vianney "acemtp" Lecroart (acemtp@gmail.com)
-- Convert normal text into h4X0r text

-- Copyright, 2005 Melting Pot.
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

	-- This is a table that contain all characters that must be translated
	ConversionTable = { a="@", b="B", c="C", d="D", e="3", f="f", g="G", h="H", i="1", j="J", k="K", l="L", m="m", n="n", o="0", p="p", q="q", r="r", s="S", t="T", u="u", v="v", w="W", x="X", y="y", z="2" }

	-- this is the final string, after the conversion
	FinalString = "";

	-- We process the whole input string, character per character
	for CurrentIndex=1, string.len(InputString) do

		-- Let extract the i-th character and and put it in the 's' variable
		CurrentCharacter = string.sub(InputString, CurrentIndex, CurrentIndex)

		-- If the character is available in the conversion table,
		if ConversionTable[CurrentCharacter] then
			-- then append it to the final string
			CurrentCharacter = ConversionTable[CurrentCharacter]
		end
		FinalString = FinalString..CurrentCharacter
	end

	-- return the final string, after conversion
	return FinalString
end
