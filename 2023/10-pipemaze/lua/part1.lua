require "pipemaze"

---@param argc int
---@param argv str[]
local function main(argc, argv)
    local lines = readfile((argc == 1 and argv[1]) or FALLBACK)
    local maze = PipeMaze.new(lines):set_first_tile()
    -- TODO: see PipeMaze:get_possible_move
    print(maze)
    repeat
        maze:update_tile():move_piece()
    until maze:is_starting_tile()
    print(maze)
    return 0
end

return main(#arg, arg)
