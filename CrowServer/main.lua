function init()
	router.handle("/", index)
end

local function isempty(s)
  return s == nil or s == ''
end

local function tablelength(T)
  local count = 0
  for _ in pairs(T) do count = count + 1 end
  return count
end

function index(params)
	f = "Hello World"
	if params["foo"] then
		f = f.." "..params["foo"]
	end
	
	
	return f
end