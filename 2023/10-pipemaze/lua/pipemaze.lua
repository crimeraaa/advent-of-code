require "util/common"
require "util/array"
require "util/math"
require "util/stdio"
require "util/string"
require "util/table"
require "util/prettyprint"

PROJECT_DIR = WORKSPACE_DIR .. convert_OSpath("/2023/10-pipemaze/")
FALLBACK = PROJECT_DIR .. "sample.txt"

---@alias DirectionStrings "north"|"south"|"east"|"west"
---@alias TileChars '|'|'-'|'L'|'J'|'7'|'F'|'.'|'S'

---@class DirectionSet
---@field direction DirectionStrings The primary direction concerned.
---@field opposite DirectionStrings Opposite/complement of primary direction.

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

function PipeMaze:set_startingtile()
    local ln, col = self.piece.ln, self.piece.col
    self.map[ln][col] = self:get_shape()
    return self
end

-- Get the 3x3 grid surrounding `self.piece` with that at the center.
-- 
-- Potential edge cases: `inst.piece` is at the corner or edges of the map.
function PipeMaze:get_searcharea(ln, col)
    local grid = {} ---@type str[][]
    for i = -1, 1 do
        local handle = {} ---@type str[]
        for ii = -1, 1 do
            local offset_ln = ln + i
            local offset_col = col + ii
            -- Avoid indexing into out of bounds indexes
            if self:is_valid_query(offset_ln, offset_col) then
                handle[#handle + 1] = self.map[offset_ln][offset_col]
            end
        end
        grid[#grid + 1] = handle
    end
    -- Only need to immediately worry about pipes to N, S, E or W, not combined.
    -- grid is of form: array[ln: int][col: int]: string
    ---@type MazeSearch
    return {
        north = grid[1][2],
        west  = grid[2][1],
        east  = grid[2][3],
        south = grid[3][2]
    }
end

-- Determine which of our immediate surroundings can be connected to us.
---@param ln int
---@param col int
function PipeMaze:get_connections(ln, col)
    -- in reference to caller pipe
    local surroundings = self:get_searcharea(ln, col) 

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

-- Although we receive a copy, we primarily use `maze` (or `self`).
-- This is because `copy` will have messed up tiles.
---@param copy PipeMaze
function PipeMaze:print_moves(copy)
    local ln, col = copy.piece.ln, copy.piece.col
    local char = self.map[ln][col]
    printf("Movements of Ln %i, Col %i ('%s'):\n", ln, col, char)
    -- TODO: Determine if the given offset is a number or not, somehow
    -- Maybe instead of number, check if it was visited previously?
    for _, set in ipairs(self.directions) do
        if self.shapes[char][set.direction] then
            printf("%s, ", set.direction)
        end
    end
end

-- Figure out the shape of the 'S' pipe based on its surroundings.
-- 
-- NOTE: Starting pipe will have exactly 2 pipes connected to it.
-- 
-- Every pipe in the main loop connects exactly 2 neighbors.
function PipeMaze:get_shape()
    local connected = self:get_connections(self.piece.ln, self.piece.col)

    -- Linear search for shape that matches our criteria
    -- TODO: maybe could be more database-like?
    for key, shape in pairs(self.shapes) do
        if MazeShape.compare(connected, shape) then
            return key
        end
    end
    return '.'
end

-- Replace the copy's current tile with the given character.
---@param copy PipeMaze
---@param char? str Default: copy's distance (in tiles) to the starting point.
function PipeMaze:set_tile(copy, char)
    local ln, col = copy.piece.ln, copy.piece.col
    local distance = copy:get_distance_to_start(ln, col)
    copy.map[ln][col] = char or tostring(distance)
    return self
end

---@param copy PipeMaze
function PipeMaze:get_move(copy)
    local ln, col = copy.piece.ln, copy.piece.col

    -- Can't use copy as it'll have invalid tile names, e.g. '1', '7', etc.
    local connected = self:get_connections(ln, col)

    for _, set in ipairs(self.directions) do
        -- TODO: Determine if the given offset is a number or not, somehow
        if connected[set.direction] then
            return set.direction
        end
    end
end

---@param copy PipeMaze
---@param direction DirectionStrings
function PipeMaze:move_piece(copy, direction)
    local move_fn = self.move_fns[direction]
    return move_fn(copy)
end

--------------------------------------------------------------------------------
---------------------------- PIPEMAZE LOOKUP TABLES ----------------------------
--------------------------------------------------------------------------------

PipeMaze.directions = {
    [1] = {direction = "north", opposite = "south"}, ---@type DirectionSet
    [2] = {direction = "west",  opposite = "east"},  ---@type DirectionSet
    [3] = {direction = "east",  opposite = "west"},  ---@type DirectionSet
    [4] = {direction = "south", opposite = "north"}, ---@type DirectionSet
}

---@param axis "ln"|"col" "ln": y-axis/line numbers, "col": x-axis/columns.
---@param offset -1|1 Offset of the chosen axis.
local function move_fn_factory(axis, offset)
    -- Try to move your piece 1 unit in the key's name's direction.
    ---@param self PipeMaze
    return function(self)
        local updated = self.piece[axis] + offset
        local bounded = self:is_in_range(updated, axis)
        -- If out of range, clip it to the current value
        self.piece[axis] = (bounded and updated) or self.piece[axis]
        return self
    end
end

PipeMaze.move_fns = {
    north = move_fn_factory("ln",  -1), -- going upwards is negative y-axis
    south = move_fn_factory("ln",  1),  -- going downwards is positive y-axis
    east  = move_fn_factory("col", 1), -- right is positive x-axis
    west  = move_fn_factory("col", -1), -- left is negative x-axis
}

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
    local distance = self:get_distance_to_start(ln, col)
    ---@type str[]
    local output = {
        string.format("{CURRENT}: '%s' (Ln %i, Col %i)", char, ln, col),
        string.format("{OFFSETS}: [%i] tiles from starting point.", distance),
        string.format("{STARTPT}: [%s]", tostring(self:is_at_start()))
    }
    for _, line in ipairs(self.map) do
        output[#output+1] = table.concat(line, " ")
    end
    return table.concat(output, "\n")
end

---@param index int Received value corresponding to a dimension to query.
---@param axis "ln"|"col" Key to index into `inst.dimensions`.
function PipeMaze:is_in_range(index, axis)
    return (index >= 1) and (index <= self.dimensions[axis])
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

function PipeMaze:is_at_start()
    -- Separating into variables purely so I can parse this at a glance better
    local ln, col = self.piece.ln, self.piece.col
    return (ln == self.start.ln) and (col == self.start.col)
end

--------------------------------------------------------------------------------
------------------------------ PIPEMAZE METATABLE ------------------------------
--------------------------------------------------------------------------------

PipeMaze.mt.__index = PipeMaze
PipeMaze.mt.__tostring = PipeMaze.tostring
