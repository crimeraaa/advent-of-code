require "util/common"
require "util/stdio"
require "util/string"
require "util/table"
require "util/prettyprint"

---@param argc int
---@param argv str[]
local function main(argc, argv)
    -- local lines = readfile((argc == 1 and argv[1]) or "./loremipsum.txt")
    local file = io.open((argc == 1 and argv[1]) or "./loremipsum.txt", "r")

    -- File pointer is moved, so we can't call `file:read` multiple times.
    ---@type str
    local contents = file:read("*a")
    local lines = contents:split("\n", true)
    local words = contents:split("%s", true)
    print_array(lines, "lines")
    print_array(words, "words")
    file:close()
    return 0
end

return main(#arg, arg)
