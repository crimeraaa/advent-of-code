require "pipemaze"

---@param argc int
---@param argv str[]
local function main(argc, argv)
    local lines = readfile((argc == 1 and argv[1]) or FALLBACK)
    -- hold our primary maze info, map must not be edited
    local maze = PipeMaze.new(lines)

    -- hold our step counts data, where tiles will be replaced with numbers
    local copy = PipeMaze.new(lines) 

    local move = maze:update(copy, maze:get_shape())
    printf("Maze:\n%s\n\n", tostring(maze))
    printf("Copy:\n%s\n\n", tostring(copy))

    --? Need this so 'F' in `copy` is replaced with '0'.
    maze:update(copy) 
    --? Do this to confirm we actually set the above properly.
    printf("Copy:\n{TO MOVE}: %s.\n%s\n\n", move, tostring(copy))

    -- TODO: Given one move, keep going? Or perhaps just move in one direction?
    -- Since it all loops back anyway this should be fine?
    maze:update(maze:move_piece(copy, move))
    printf("Copy:\n{TO MOVE}: %s.\n%s\n\n", move, tostring(copy))
    return 0
end

return main(#arg, arg)
