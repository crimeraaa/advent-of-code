require "pipemaze"

---@param argc int
---@param argv str[]
local function main(argc, argv)
    local lines = readfile((argc == 1 and argv[1]) or FALLBACK)
    local maze = PipeMaze:new(lines)
    for _, line in ipairs(maze.map) do
        print(table.concat(line, " "))
    end
    -- TODO: Find connections to current location, then recurse until we reach
    -- the starting point again?
    -- Maybe need a copy of the maze to keep track of distances from start.
    printf("'S' @ Ln %i, Col %i\n", maze.piece.ln, maze.piece.col)
    maze:get_shape()
    return 0
end

return main(#arg, arg)
