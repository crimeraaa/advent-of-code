-- NOTE:    Adapted from William Feng's solution.
--          His is much smarter than mine! I had no idea what a BFS was before.
-- LINK:    https://youtu.be/zhmzPQwgPg0?si=wHjiQtLfrzL3f1Kv
require "pipemaze-take2"

---------------------------------- GLOBALS -------------------------------------

local MAZE = {map={}, visited = {}} ---@type Maze 
-- Initialize maze already so we have a valid global to work with
for _, line in ipairs(readfile((#arg == 1 and arg[1]) or FALLBACK) ) do
    MAZE.map[#MAZE.map+1] = line:toarray()
    MAZE.visited[#MAZE.visited+1] = array.new(#line, false)
end
local DIMENSIONS = {ln=#MAZE.map, col=#MAZE.map[1]}

-- Form: `{{ln, col}, {ln, col}...}`
local NEIGHBORS = { ---@type NewDict<TileChars, Neighbor[]>
    ['|'] = {{ln=1, col=0}, {ln=-1, col=0}}, -- 1 down && 1 above
    ['-'] = {{ln=0, col=1}, {ln=0, col=-1}}, -- 1 right && 1 left
    ['L'] = {{ln=-1, col=0}, {ln=0, col=1}}, -- 1 above && 1 right
    ['J'] = {{ln=-1, col=0}, {ln=0, col=-1}}, -- 1 above && 1 left
    ['7'] = {{ln=1, col=0}, {ln=0, col=-1}}, -- 1 down, 1 left
    ['F'] = {{ln=1, col=0}, {ln=0, col=1}}, -- 1 down, 1 right
    ['.'] = {}, -- no connections so don't bother
    -- 1 down && 1 above && 1 right && 1 left
    ['S'] = {{ln=1, col=0}, {ln=-1, col=0}, {ln=0, col=1}, {ln=0, col=-1}},
}

----------------------------- HELPER FUNCTIONS ---------------------------------

local function is_bounded(index, axis) -- `axis` must be one of "ln" or "col"
    return (index >= 1 and index <= DIMENSIONS[axis])
end

local function is_valid_offset(ln, col)
    return is_bounded(ln, "ln") and is_bounded(col, "col")
end

local function is_connected(ln, col, neighbor)
    return ln == neighbor.ln and col == neighbor.col
end

---------------------------- NEIGHBOR FUNCTIONS --------------------------------

-- given pipe's w/ specific coords, dump its neighbors' absolute locations
local function get_absneighbors(ln, col) 
    local char = MAZE.map[ln][col]
    local connections = {} ---@type Neighbor[]
    -- Loop through this character's possible relative offsets
    for _, neighbor in ipairs(NEIGHBORS[char]) do
        local offsetln, offsetcol = (ln + neighbor.ln), (col + neighbor.col)
        if is_valid_offset(offsetln, offsetcol) then
            connections[#connections+1] = {ln=offsetln, col=offsetcol}
        end
    end
    return connections
end

local function get_subneighbors(ln, col, neighbor)
    local connects = {} ---@type Neighbor[]
    local testln, testcol = (ln + neighbor.ln), (col + neighbor.col)
    if is_valid_offset(testln, testcol) then
        local subneighbors = get_absneighbors(testln, testcol)
        for _, subneighbor in ipairs(subneighbors) do
            -- Does this subneighbor connect to our caller in some way?
            if is_connected(ln, col, subneighbor) then
                connects[#connects+1] = {ln = neighbor.ln, col = neighbor.col}
            end
        end
    end
    return connects
end

-- try to determine shape and what pipes connect to us
local function get_startneighbors(ln, col)
    local connects = {} ---@type Neighbor[]
    -- Will test all 4 possible neighbors: north, south, east, west
    for _, neighbor in ipairs(NEIGHBORS['S']) do
        -- If subneighbor connects to us at any point, append their connections
        for _, connection in ipairs(get_subneighbors(ln, col, neighbor)) do
            connects[#connects+1] = connection
        end
    end
    return connects
end

-- dump relative positions of valid neighbors in relation to given coordinates
-- 
-- (computes differential of the pipe at given coords)
local function get_relneighbors(ln, col) 
    local char = MAZE.map[ln][col]
    return (char == 'S' and get_startneighbors(ln, col)) or NEIGHBORS[char]
end

local function get_startpos() 
    for ln = 1, DIMENSIONS.ln do
        for col = 1, DIMENSIONS.col do
            if MAZE.map[ln][col] == 'S' then
                return {ln=ln, col=col} ---@type Location
            end
        end
    end
    return {ln=0, col=0}
end

------------------------------- PRINOUT FUNCTIONS ------------------------------

-- local function print_maze(copy, current, distance, steps)
--     for _, row in ipairs(copy) do
--         printf("%s\n", table.concat(row, " "))
--     end
--     local ln, col = current.ln, current.col
--     printf("'%s' (Ln %i, Col %i)\n", MAZE.map[ln][col], ln, col)
--     printf("{DISTANCE}: %i\n", distance)
--     printf("{NUMSTEPS}: %i\n", steps)
--     printf("{NEIGHBOR}: ")
--     for _, neighbor in ipairs(get_relneighbors(ln, col)) do
--         local absln, abscol = (ln + neighbor.ln), (col + neighbor.col)
--         local char = MAZE.map[absln][abscol]
--         printf("'%s', ", char)
--     end
--     printf("\n----------------\n")
-- end

------------------------------ SEARCH FUNCTIONS --------------------------------

-- local function get_distance(current, start)
--     return math.abs((current.ln - start.ln) + (current.col - start.col))
-- end

-- local function get_nextlocation(current, start)
--     local ln, col = current.ln, current.col
--     MAZE.visited[ln][col] = true
--     -- Get the first possible move on a neighbor that hasn't been visited yet
--     for _, neighbor in ipairs(get_relneighbors(ln, col)) do
--         local absln, abscol = (ln + neighbor.ln), (col + neighbor.col)
--         local visited = MAZE.visited[absln][abscol]
--         if is_valid_offset(absln, abscol) and not visited then
--             return {ln=absln, col=abscol} ---@type Location
--         end
--     end
--     return start
-- end

------------------------------- SCRIPT PROPER ----------------------------------

-- local start = get_startpos();
-- local current = {ln=start.ln, col=start.col} ---@type Location
-- local steps = 0
-- local distance = 0

-- -- need a copy so MAZE.map can still be used in lookups for neighbors
-- local stepstaken = table.copy(MAZE.map, function(v) return table.copy(v) end)
-- print_maze(stepstaken, current, distance, steps)

-- -- Starting point will be marked visited, so keep going until we hit it again
-- repeat
--     stepstaken[current.ln][current.col] = distance
--     steps = steps + 1

--     -- will mark current point, especially at starting point, as visited
--     current = get_nextlocation(current, start)

--     -- gets raw distance (not considering path) to starting point
--     distance = get_distance(current, start)
-- until MAZE.visited[current.ln][current.col]

-- print_maze(stepstaken, current, distance, steps)

-- printf("The farthest point is %i steps away.\n", math.floor(steps / 2))


------------------------------- DEQUE SOLUTION ---------------------------------
-- Do a breadth-first-search (BFS)

---@alias MapQueue {info:Location, dist:int}

-- Don't include distances in the hash as they'll mess it up
-- need this as hashing local tables always results in different hashes
local function mapqueue_tostring(mq) ---@param mq MapQueue
    local ln, col = mq.info.ln, mq.info.col
    return string.format("Ln %i, Col %i", ln, col)
end

-- `mapqueue_tostring` "hashes" MapQueue instances so they can be looked up.
local visited = {} ---@type { str:bool }

-- first element is just starting position w/ default distance of 0
local start = {info=get_startpos(), dist=0} ---@type MapQueue

-- internal buffer is a 1D array of MapQueues
local dq = deque.new(start) 

-- Do this just so we don't waste 1 cycle later on
visited[mapqueue_tostring(start)] = true

local function queue_neighbors(ln, col, dist) 
    for i, neighbor in ipairs(get_relneighbors(ln, col)) do
        local absln, abscol = (ln + neighbor.ln), (col + neighbor.col)
        -- Adding 1 helps us keep track of steps for this particular path,
        -- certainly beats somehow manually calculating the distance...
        ---@type MapQueue
        local test = {
            info = {ln = absln, col = abscol}, 
            dist = dist + 1
        }
        local hash = mapqueue_tostring(test)
        -- Paths who were faster to reach here would've already marked this,
        -- and it's likely their values were greater.
        if not visited[hash] then
            visited[hash] = true
            dq:push_right(test)
        end
    end
end

for _, row in ipairs(MAZE.map) do
    printf("%s\n", table.concat(row, " "))
end

local maxsteps = 0

local copy = table.copy(MAZE.map, function(v) return table.copy(v) end)
while dq:len() > 0 do
    local popped = dq:pop_left() ---@type MapQueue
    local ln, col, dist = popped.info.ln, popped.info.col, popped.dist
    local char = MAZE.map[ln][col]
    copy[ln][col] = dist
    queue_neighbors(ln, col, dist)
    printf("'%s': Ln %i, Col %i, Dist %i\n", char, ln, col, dist)
    if dist > maxsteps then
        maxsteps = dist
    end
end

for _, row in ipairs(copy) do
    printf("%s\n", table.concat(row, " "))
end

printf("Farthest point: %i steps\n", maxsteps)
