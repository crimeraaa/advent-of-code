require "pipemaze"

---@param argc int
---@param argv str[]
local function main(argc, argv)
    local lines = readfile((argc == 1 and argv[1]) or FALLBACK)
    -- hold our primary maze info, map must not be edited
    local maze = PipeMaze:new(lines)

    -- hold our step counts data, where tiles will be replaced with numbers
    local steps = PipeMaze:new(lines) 

    -- TODO: Find connections to current location, then recurse until we reach
    -- the starting point again?
    -- Maybe need a copy of the maze to keep track of distances from start.
    steps:update(maze:get_shape())
    for _, line in ipairs(steps.map) do
        print(table.concat(line, " "))
    end
    return 0
end

return main(#arg, arg)
