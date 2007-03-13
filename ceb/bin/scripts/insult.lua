-- rot13.lua
-- By Drealmer
-- Description here

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

function ReplaceSub(Input, Sub1, Sub2)
  pos = string.find(Input, Sub1, 0, true)
  if(pos == nil) then return Input end
  left = string.sub(Input, 0, pos - 1)
  right = string.sub(Input, pos + string.len(Sub1))
  return left..Sub2..right
end

function InputFilter(InputString)

  adj = {"pauvre", "misérable", "lamentable", "triple", "sombre", "abominable", "effroyable"}
  noun = {"con", "abruti", "débile", "connard", "merde", "naze", "déchet", "tête de noeud", "bâtard", "enxulé", "idiot", "crétin"}

  result = InputString
  while(true) do
    ai = math.random(table.getn(adj))
    ni = math.random(table.getn(noun))
    result = ReplaceSub(result, "*insult*", adj[ai].." "..noun[ni])
    if(string.find(result, "*insult*", 0, true) == nil) then break end
  end

  return result;
end