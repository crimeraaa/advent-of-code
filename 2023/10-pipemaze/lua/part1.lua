require "pipemaze"

---@param argc int
---@param argv str[]
local function main(argc, argv)
    local lines = readfile((argc == 1 and argv[1]) or FALLBACK)
    -- hold our primary maze info, map must not be edited (for the most part)
    local maze = PipeMaze.new(lines):set_startingtile()

    -- hold our step counts data, where tiles will be replaced with numbers
    local copy = PipeMaze.new(lines):set_startingtile()

    -- Get the first move to make, 1 of 2 but it's a loop so it doesn't matter.
    -- This also set's copy's starting tile to '0'.
    local move = maze:set_tile(copy):get_move(copy)
    
    printf("Maze:\n%s\n\n", tostring(maze))
    printf("Copy:\n{TO MOVE}: %s.\n%s\n", move, tostring(copy))

    -- TODO: We'll have to determine which of our 2 possible connections to use.
    -- Because as is, this gets the first match which may not be what we want.
    move = maze:set_tile(copy):get_move(maze:move_piece(copy, move))
    printf("\nCopy:\n{TO MOVE}: %s.\n%s\n", move, tostring(copy))
    maze:print_moves(copy)
    return 0
end

return main(#arg, arg)
