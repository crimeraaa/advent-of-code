require "util/common"
require "util/array"
require "util/math"
require "util/stdio"
require "util/string"
require "util/table"
require "util/prettyprint"

PROJECT_DIR = WORKSPACE_DIR .. convert_OSpath("/2023/10-pipemaze/")
FALLBACK = PROJECT_DIR .. "sample.txt"

---@alias Directions "north"|"south"|"east"|"west"
---@alias TilesChars '|'|'-'|'L'|'J'|'7'|'F'|'.'|'S'

---@class MazeCoord
---@field ln int Line number.
---@field col int Index into line `ln`.

---@class MazeSize
---@field ln int How many horizontal lines there are.
---@field col int How many columns in a line there are.

---@class MazeSearch
---@field north str Pipe above our current piece.
---@field south str Pipe below our current piece.
---@field east str Pipe right of our current piece.
---@field west str Pipe left of our current piece.

---@class MazeShape
---@field north bool If this piece placed above caller makes a connection.
---@field south bool If this piece placed below caller makes a connection.
---@field east bool If this piece placed right of caller makes a connection.
---@field west bool If this piece placed left of caller makes a connection.

MazeShape = {}
function MazeShape.new()
    ---@type MazeShape
    return {
        north = false, south = false, 
        east  = false, west  = false
    }
end

---@param left MazeShape
---@param right MazeShape
function MazeShape.compare(left, right)
    for key in pairs(left) do
        if left[key] ~= right[key] then
            return false
        end
    end
    return true
end

---@class PipeMaze
---@field map str[][] Array of lines, which are in turn arrays of chars.
---@field piece MazeCoord Location of 'S' char in the maze.
---@field dimensions MazeSize So we don't have to use '#' operator a whole lot.
PipeMaze = {}
PipeMaze.mt = {}

---@param lines str[]
function PipeMaze:new(lines)
    -- Upvalue line counter so callback function can correctly set `piece`.
    local line_number = 0

    ---@type MazeCoord
    local piece = {col=0, ln=0} 

    ---@type str[][]
    local map = table.copy(lines, function(v)
        line_number = line_number + 1
        local line_column = string.find(v, 'S')
        if line_column then
            piece.ln, piece.col = line_number, line_column
        end
        return string.toarray(v) 
    end)

    ---@type PipeMaze
    local inst = {
        piece = piece,
        map = map,
        dimensions = {ln = #lines, col = #lines[1]},
    }
    return setmetatable(inst, PipeMaze.mt)
end

function PipeMaze:is_valid_query(ln, col)
    local valid_ln = (ln >= 1 and ln <= self.dimensions.ln)
    local valid_col = (col >= 1 and col <= self.dimensions.col)
    return valid_ln and valid_col
end

-- Get the 3x3 grid surrounding `self.piece` with that at the center.
-- 
-- Potential edge cases: `self.piece` is at the corner or edges of the map.
function PipeMaze:get_searcharea()
    ---@type str[][]
    local grid = {}
    for i = -1, 1 do
        ---@type str[]
        local handle = {}
        for ii = -1, 1 do
            local ln = self.piece.ln + i
            local col = self.piece.col + ii
            -- Avoid indexing into out of bounds indexes
            if self:is_valid_query(ln, col) then
                handle[#handle + 1] = self.map[ln][col]
            end
        end
        grid[#grid + 1] = handle
    end
    -- Only need to immediately worry about pipes to N, S, E or W, not combined.
    -- grid is of form: str_array[ln][col]
    ---@type MazeSearch
    return {
        north = grid[1][2],
        west = grid[2][1],
        east = grid[2][3],
        south = grid[3][2]
    }
end

-- Check if the callee can connect to the caller (not the other way around!)
---@param direction Directions `MazeSearch`/`MazeShape` keys.
---@param pipe TilesChars Current pipe character, see `MazePipe.shapes`.
function PipeMaze:is_connection(direction, pipe)
    printf("%-6s %s", string.format("%s:", direction), pipe)
    -- (direction="north",pipe='|') should pass
    -- but (direction="south", pipe='J') shouldn't
    local connects = self.shapes[pipe][direction]
    if connects then
        printf("\tProbably connects %s", direction)
    end
    printf("\n")
    return connects
end

function PipeMaze:get_query()
    local search = self:get_searcharea() -- in reference to caller pipe
    local query = MazeShape.new() -- in reference to callee pipe
    -- Remember this part is from callee pipe's POV, it's really stupid to do it
    -- this way but it works for all sample, complex and input it seems...
    query.north = self:is_connection("south", search.north)
    query.south = self:is_connection("north", search.south)
    query.east  = self:is_connection("west", search.east)
    query.west  = self:is_connection("east", search.west)
    return query
end

-- Figure out the shape of the 'S' pipe based on its surroundings.
-- 
-- NOTE: Starting pipe will have exactly 2 pipes connected to it.
-- 
-- Every pipe in the main loop connects exactly 2 neighbors.
function PipeMaze:get_shape()
    local query = self:get_query()

    -- Linear search for shape that matches our criteria
    -- TODO: maybe could be more database-like?
    for key, shape in pairs(self.shapes) do
        if MazeShape.compare(query, shape) then
            printf("Probably is %s\n", key)
            return key
        end
    end
    return '.'
end

-- Possible pipe shapes for a tile and what directions leading to them are valid.
-- Note that our booleans for bending pipes are reversed.
-- 
-- This is because the MazeShape fields are based on if the PIPE were to
-- be placed in some direction in relation to the CALLER, not vice versa.
PipeMaze.shapes = {
    ---@type MazeShape vertical pipe connecting north and south.
    -- Connects to either above or below caller.
    ['|'] = {
        north = true,  south = true, 
        east  = false, west  = false
    },
    ---@type MazeShape horizontal pipe connecting east and west.
    -- Connects to either left or right of caller.
    ['-'] = {
        north = false, south = false, 
        east  = true,  west  = true,
    }, 
    ---@type MazeShape 90-degree bend connecting north and east.
    -- Connects to either below caller or left of caller.
    ['L'] = {
        north = true, south = false, 
        east  = true, west  = false
    }, 
    ---@type MazeShape 90-degree bend connecting north and west.
    -- Connects to either below caller or right of caller.
    ['J'] = {
        north = true,  south = false,
        east  = false, west  = true,
    }, 
    ---@type MazeShape 90-degree bend connecting south and west.
    -- Connects to either above caller or right of caller.
    ['7'] = {
        north = false, south = true, 
        east  = false, west  = true,
    }, 
    ---@type MazeShape 90-degree bend connecting south and east.
    -- Connects to either above caller or left of caller.
    ['F'] = {
        north = false, south = true,
        east  = true,  west  = false,
    }, 
    ---@type MazeShape ground. there is no pipe in this tile.
    -- Does not connect to caller in any way because it's not a pipe.
    ['.'] = {
        north = false, south = false, 
        east  = false, west  = false
    }, 
    ---@type MazeShape starting position of the animal; there's a pipe here
    -- but your sketch doesn't show what shape the pipe has!
    -- Does not connect to caller in any way because it's not a pipe.
    ['S'] = {
        north = false, south = false, 
        east  = false, west  = false
    },
}

--------------------------------------------------------------------------------
------------------------------ METATABLE DETAILS -------------------------------
--------------------------------------------------------------------------------

PipeMaze.mt.__index = PipeMaze
