-- rot13.lua
-- By Benjamin "ben" Legros
-- Convert normal text into rot13 text

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

       -- this is the final string, after the conversion
       local   FinalString = ""

       local   infLo = string.byte('a')
       local   supLo = string.byte('z')
       local   infHi = string.byte('A')
       local   supHi = string.byte('Z')

       -- We process the whole input string, character per character
       for CurrentIndex=1, string.len(InputString) do

               local   cbyte = string.byte(string.sub(InputString, CurrentIndex, CurrentIndex));

               if cbyte >= infLo and cbyte <= supLo then
                       cbyte = math.mod(cbyte - infLo + 13, 26) + infLo
               elseif cbyte >= infHi and cbyte <= supHi then
                       cbyte = math.mod(cbyte - infHi + 13, 26) + infHi
               end

               FinalString = FinalString..string.char(cbyte)
       end

       -- return the final string, after conversion
       return FinalString
end