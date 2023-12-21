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

------------------------------ SEARCH FUNCTIONS --------------------------------

local function get_current_neighbors(location)
    local ln, col = location.ln, location.col
    local neighbors = get_relneighbors(ln, col)
    printf("'%s' (Ln %i, Col %i)\n", MAZE.map[ln][col], ln, col)
    for i, neighbor in ipairs(neighbors) do
        -- since neighbor only have relative values, use params to get absolute
        local absln, abscol = (ln + neighbor.ln), (col + neighbor.col)
        local char = MAZE.map[absln][abscol]
        printf("Neighbor %i: '%s' (Ln %i, Col %i)\n", i, char, absln, abscol)
        printf("    Offsets:     Ln %i, Col %i\n", neighbor.ln, neighbor.col)
    end
    return neighbors
end

------------------------------- SCRIPT PROPER ----------------------------------

for _, row in ipairs(MAZE.map) do
    printf("%s\n", table.concat(row, " "))
end

local start = get_startpos()
local current = {ln=start.ln, col=start.col} ---@type Location
get_current_neighbors(current)
