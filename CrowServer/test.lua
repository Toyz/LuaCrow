function init()
	router.handle("/", "index")
end

local function isempty(s)
  return s == nil or s == ''
end

function index(params, url)
	local f = "Hello World"
	
	print(type(params))
	
	return f
end