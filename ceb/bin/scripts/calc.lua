function InputFilter(str)
 g = assert(loadstring("return "..str))()
 if g == nil then
   return str
 else
   return str.." = "..g
 end
end
