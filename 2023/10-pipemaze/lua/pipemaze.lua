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
---@alias TileChars '|'|'-'|'L'|'J'|'7'|'F'|'.'|'S'

---@class MazeCoord
---@field ln int Line number.
---@field col int Index into line `ln`.

---@class MazeSize
---@field ln int How many horizontal lines there are.
---@field col int How many columns in a line there are.

---@class MazeSearch
---@field north TileChars Pipe above our current piece.
---@field south TileChars Pipe below our current piece.
---@field east TileChars Pipe right of our current piece.
---@field west TileChars Pipe left of our current piece.

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
---@field piece MazeCoord Location of the current character in the maze.
---@field start MazeCoord Location of the starting point so we know where to stop.
---@field dimensions MazeSize So we don't have to use '#' operator a whole lot.
PipeMaze = {}

-- Metatable. Do not modify unless you know exactly what you're doing!
PipeMaze.mt = {}

--------------------------------------------------------------------------------
--------------------- PIPEMAZE PRIMARY USER-SIDED METHODS ----------------------
--------------------------------------------------------------------------------

---@param lines str[]
function PipeMaze.new(lines)
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

    -- Both the current and starting point are the same piece at first.
    ---@type PipeMaze
    return setmetatable({
        piece = piece,
        start = table.copy(piece), -- Copy by value as piece will be updated.
        map = map,
        dimensions = {ln = #lines, col = #lines[1]},
    }, PipeMaze.mt)
end

-- Get the 3x3 grid surrounding `self.piece` with that at the center.
-- 
-- Potential edge cases: `inst.piece` is at the corner or edges of the map.
function PipeMaze:get_searcharea()
    local grid = {} ---@type str[][]
    for i = -1, 1 do
        local handle = {} ---@type str[]
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

-- Determine which of our immediate surroundings can be connected to us.
function PipeMaze:get_connections()
    -- in reference to caller pipe
    local surroundings = self:get_searcharea() 

    -- Check if the callee can connect to the caller (not the other way around!)
    -- Seems really stupid to do this way but it works...
    -- 
    -- What's happening here is: does the piece to the caller's north
    -- allow for a connection to it (the piece's) south? etc. etc.
    ---@type MazeShape
    return {
        north = self.shapes[surroundings.north]["south"],
        south = self.shapes[surroundings.south]["north"],
        east  = self.shapes[surroundings.east]["west"],
        west  = self.shapes[surroundings.west]["east"],
    }
end

-- Figure out the shape of the 'S' pipe based on its surroundings.
-- 
-- NOTE: Starting pipe will have exactly 2 pipes connected to it.
-- 
-- Every pipe in the main loop connects exactly 2 neighbors.
function PipeMaze:get_shape()
    local connected = self:get_connections()

    -- Linear search for shape that matches our criteria
    -- TODO: maybe could be more database-like?
    for key, shape in pairs(PipeMaze.shapes) do
        if MazeShape.compare(connected, shape) then
            return key
        end
    end
    return '.'
end

-- Mainly meant for copies to replace the current 'S' character.
-- 
-- Will also be used by copies to replace the current piece with a number.
-- 
-- TODO: move `inst.piece`... somehow. There are always 2 possible pieces.
-- TODO: Maybe first get the distance to the starting piece?
-- TODO: Need to keep maze and copy separate.
---@param char? str
function PipeMaze:update(copy, char)
    local distance = copy:get_distance_to_start(copy.piece.ln, copy.piece.col)
    printf("Distance from start: %i\n", distance)
    copy.map[copy.piece.ln][copy.piece.col] = char or tostring(distance)
    local moves = {} ---@type str[]
    local connected = PipeMaze.get_connections(self)
    for _, direction in ipairs{"north", "west", "east", "south"} do
        if connected[direction] then
            moves[#moves + 1] = direction
        end
    end
    return moves
end

--------------------------------------------------------------------------------
---------------------------- PIPEMAZE LOOKUP TABLES ----------------------------
--------------------------------------------------------------------------------

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
--------------------------- PIPEMAZE HELPER METHODS ----------------------------
--------------------------------------------------------------------------------

-- Convert your current map to a giant string, mainly for debug purposes.
function PipeMaze:tostring()
    local ln, col = self.piece.ln, self.piece.col
    local char = self.map[ln][col]
    ---@type str[]
    local output = {
        [1] = string.format("Current: '%s' @(Ln %i, Col %i)", char, ln, col),
    }
    for _, line in ipairs(self.map) do
        output[#output+1] = table.concat(line, " ")
    end
    return table.concat(output, "\n")
end

---@param index int Received value corresponding to a dimension to query.
---@param limit "ln"|"col" Key to index into `inst.dimensions`.
function PipeMaze:is_in_range(index, limit)
    return index >= 1 and index <= self.dimensions[limit]
end

---@param ln int
---@param col int
function PipeMaze:is_valid_query(ln, col)
    return self:is_in_range(ln, "ln") and self:is_in_range(col, "col")
end

---@param ln int
---@param col int
function PipeMaze:get_distance_to_start(ln, col)
    return math.abs((self.start.ln - ln) - (self.start.col - col))
end

--------------------------------------------------------------------------------
------------------------------ PIPEMAZE METATABLE ------------------------------
--------------------------------------------------------------------------------

PipeMaze.mt.__index = PipeMaze
PipeMaze.mt.__tostring = PipeMaze.tostring
