require "util/common"
require "util/array"
require "util/math"
require "util/stdio"
require "util/string"
require "util/table"
require "util/prettyprint"

PROJECT_DIR = WORKSPACE_DIR .. convert_OSpath("/2023/10-pipemaze/")
FALLBACK = PROJECT_DIR .. "sample.txt"

---@class PipeMaze
---@field map PipeChars[][] Tile data or distance (in tiles) to start.
---@field visited bool[][] Keep track of which tiles were previously visited.
---@field start MapInfo Starting piece's coordinates. (Mostly) read-only.
---@field piece MapInfo Current piece's coordinates, Is mutable/modifiable.
---@field dimensions MapInfo How big the map is, assumes it's a rectangle.
PipeMaze = {}

---------------------------- PIPEMAZE LOOKUP TABLES ----------------------------

-- List of keys (pipe shapes), each states which directions can be connected.
PipeMaze.shapes = require("pipedatabase").shapes

-- List of directions and their complements, e.g. key "north" maps to "south".
PipeMaze.directions = require("pipedatabase").directions

-- List of piecement movement functions, query via a direction string.
PipeMaze.move_piece_fns = require("pipedatabase").move_piece_fns

---------------------------- PIPEMAZE IMPLEMENTATION ---------------------------

function PipeMaze.new(lines)
    local strlen = #lines[1]
    ---@type PipeMaze
    local inst = {
        map = {}, 
        visited = {}, 
        start = {ln=0, col=0},
        piece = {ln=0, col=0},
        dimensions = {ln=#lines, col=strlen},
    }
    for i, line in ipairs(lines) do
        local col = line:find('S') -- assuming only one 'S' exists
        if col then
            inst.start.ln = i
            inst.start.col = col
            inst.piece = table.copy(inst.start)
        end
        inst.map[i] = line:toarray()
        inst.visited[i] = array.new(strlen, false)
    end
    return setmetatable(inst, PipeMaze.mt)
end

function PipeMaze:set_first_tile()
    local ln, col = self.start.ln, self.start.col
    self.map[ln][col] = self:get_shape(ln, col)
    return self
end

function PipeMaze:get_shape(ln, col) -- mainly to initialize starting position
    local neighbors = self:get_neighbors(ln, col)
    local connected = self:get_valid_connections(neighbors)
    for key, shape in pairs(self.shapes) do
        -- Both pipe-infos have the exact same booleans so we got a 100% match.
        if self:compare_pipes(connected, shape) == true then
            return key
        end
    end
    return '.'
end

function PipeMaze:get_neighbors(ln, col)
    local grid = {} ---@type MapInfo[][]
    for i = -1, 1 do
        local line = {} ---@type MapInfo[]
        for ii = -1, 1 do
            local offset_ln = ln + i
            local offset_col = col + ii
            if self:is_valid_offset(offset_ln, offset_col) then
                line[#line + 1] = {ln = offset_ln, col = offset_col}
            end
        end
        grid[#grid+1] = line
    end
    -- Ignore the other directions (e.g. northeast, southwest).
    ---@type NeighborSet
    return {
        north = grid[1][2],
        west  = grid[2][1],
        east  = grid[2][3],
        south = grid[3][2],
    }
end

function PipeMaze:get_valid_connections(neighbors) ---@param neighbors NeighborSet
    ---@type PipeInfo
    local connected = {north = false, west = false, east = false, south = false} 

    -- Does their caller's north neighbor allow for a connection  to its south?
    -- Remember this is in relation to the neighbor, not the caller.
    for direction, opposite in pairs(self.directions) do
        local neighbor = neighbors[direction]
        local char = self.map[neighbor.ln][neighbor.col]
        local visited = self.visited[neighbor.ln][neighbor.col]
        connected[direction] = (not visited) and self.shapes[char][opposite]
    end
    return connected
end

-- First possible move for this current tile, in no particular order!
function PipeMaze:get_possible_move()
    local ln, col = self.piece.ln, self.piece.col
    local char = self.map[ln][col]
    -- Only need main key, no need for the complement/opposite directions
    for direction in pairs(self.directions) do
        if self.shapes[char][direction] == true then
            return direction
        end
    end
end

function PipeMaze:update_tile()
    local ln, col = self.piece.ln, self.piece.col
    local distance = self:get_distance(ln, col)
    self.map[ln][col] = distance
    self.visited[ln][col] = true
    return self
end

function PipeMaze:move_piece(direction) ---@param direction DirectionStrs
    local move_fn = self.move_piece_fns[direction]
    return move_fn(self)
end


------------------------------- PIPEMAZE HELPERS -------------------------------

function PipeMaze:is_valid_offset(ln, col)
    return self:is_in_bounds(ln, "ln") and self:is_in_bounds(col, "col")
end

function PipeMaze:is_in_bounds(index, axis)
    return (index >= 1) and (index <= self.dimensions[axis])
end

---@param left PipeInfo
---@param right PipeInfo
function PipeMaze:compare_pipes(left, right)
    -- Just use the main key, no need for the complement direction in this case.
    for direction in pairs(self.directions) do
        if left[direction] ~= right[direction] then
            return false
        end
    end
    return true
end

function PipeMaze:get_distance(ln, col)
    return math.abs(self.start.ln - ln + self.start.col - col) ---@type int
end

------------------------------ PIPEMAZE METATABLE ------------------------------

function PipeMaze:tostring()
    local ln, col = self.piece.ln, self.piece.col
    local char = self.map[ln][col]
    local distance = self:get_distance(ln, col)
    local output = {
        string.format("{CURRENT}:  '%s' (Ln %i, Col %i)", char, ln, col),
        string.format("{VISITED}:  %s", tostring(self.visited[ln][col])),
        string.format("{DISTANCE}: %i units from start.", distance),
    }
    for _, line in ipairs(self.map) do
        output[#output + 1] = table.concat(line, " ")
    end
    return table.concat(output, "\n")
end

PipeMaze.mt = { ---@type metatable
    __index = PipeMaze,
    __tostring = PipeMaze.tostring,
}
