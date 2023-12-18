require "pipemaze"

---@param argc int
---@param argv str[]
local function main(argc, argv)
    local lines = readfile((argc == 1 and argv[1]) or FALLBACK)
    local maze = PipeMaze:new(lines)
    for _, line in ipairs(maze.tiles) do
        print(table.concat(line, " "))
    end
    printf("'S' @ Ln %i, Col %i\n", maze.piece.ln, maze.piece.col)
    return 0
end

return main(#arg, arg)
