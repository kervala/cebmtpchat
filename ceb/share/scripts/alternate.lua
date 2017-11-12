-- alternate.lua
-- By Guillaume Denry (guillaume.denry@gmail.com)
-- exemple: "hello world" => "HeLlO WoRlD"

function InputFilter(str)
  gr = "";
  for i=1, string.len(str) do
    s = string.sub(str, i, i)
    if math.mod(i, 2) == 0 then
      gr = gr..string.upper(s)
    else
      gr = gr..string.lower(s)
    end
  end

  return gr
end
