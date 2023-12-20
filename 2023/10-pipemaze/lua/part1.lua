require "pipemaze"

---@param argc int
---@param argv str[]
local function main(argc, argv)
    local lines = readfile((argc == 1 and argv[1]) or FALLBACK)
    local maze = PipeMaze.new(lines):set_first_tile()
    print(maze)
    local move = maze:get_possible_move()
    printf("Try to move to the %s!\n", move)
    print(maze:update_tile():move_piece(move))
    return 0
end

return main(#arg, arg)
