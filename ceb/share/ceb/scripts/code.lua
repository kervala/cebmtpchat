-- code.lua
-- By Ulukyn
-- Preserve indentations.

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
	return InputString:gsub("\t", "  ")
end
