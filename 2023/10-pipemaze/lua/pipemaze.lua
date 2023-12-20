require "util/common"
require "util/array"
require "util/math"
require "util/stdio"
require "util/string"
require "util/table"
require "util/prettyprint"

PROJECT_DIR = WORKSPACE_DIR .. convert_OSpath("/2023/10-pipemaze/")
FALLBACK = PROJECT_DIR .. "complex.txt"

---@class PipeMaze
---@field map PipeChars[][] Tile data or distance (in tiles) to start.
---@field steps int Highest number of steps.
---@field visited bool[][] Keep track of which tiles were previously visited.
---@field start MapInfo Starting piece's coordinates. (Mostly) read-only.
---@field piece MapInfo Current piece's coordinates, Is mutable/modifiable.
---@field direction DirectionStrs Which direction we want to go to currently.
---@field dimensions MapInfo How big the map is, assumes it's a rectangle.
PipeMaze = {}

---------------------------- PIPEMAZE LOOKUP TABLES ----------------------------

-- List of keys (pipe shapes), each states which directions can be connected.
PipeMaze.shapes = require("pipedatabase").shapes

-- List of directions and their complements, e.g. key "north" maps to "south".
PipeMaze.directions = require("pipedatabase").directions

-- List of piecement movement functions, query via a direction string.
PipeMaze.move_piece_fns = require("pipedatabase").move_piece_fns

PipeMaze.move_test_fns = require("pipedatabase").move_test_fns

---------------------------- PIPEMAZE IMPLEMENTATION ---------------------------

function PipeMaze.new(lines)
    local strlen = #lines[1]
    ---@type PipeMaze
    local inst = {
        map = {}, 
        steps = 0,
        direction = "north",
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
    self.direction = self:get_possible_move()
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
    -- Ignore the other directions (e.g. northeast, southwest).
    -- TODO: how to deal with current piece on corners and/or edges?
    ---@type NeighborSet
    return {
        north = {ln = ln - 1, col = col},
        west  = {ln = ln,     col = col - 1},
        east  = {ln = ln,     col = col + 1},
        south = {ln = ln + 1, col = col},
    }
end

function PipeMaze:get_valid_connections(neighbors) ---@param neighbors NeighborSet
    ---@type PipeInfo
    local connected = {north = false, west = false, east = false, south = false} 

    -- Does their caller's north neighbor allow for a connection  to its south?
    -- Remember this is in relation to the neighbor, not the caller.
    for direction, opposite in pairs(self.directions) do
        local neighbor = neighbors[direction]
        if neighbor.ln ~= 0 and neighbor.col ~= 0 then
            local char = self.map[neighbor.ln][neighbor.col]
            local visited = self.visited[neighbor.ln][neighbor.col]
            connected[direction] = (not visited) and self.shapes[char][opposite]
        end
    end
    return connected
end

-- First possible move for this current tile, in no particular order!
function PipeMaze:get_possible_move()
    
    local ln, col = self.piece.ln, self.piece.col
    local char = self.map[ln][col]
    -- TODO: How to avoid crashing when the start is reached again?
    if self:is_starting_tile() and self.visited[ln][col] then
        return nil
    end
    -- Only need main key, no need for the complement/opposite directions
    for direction in pairs(self.directions) do
        if self.shapes[char][direction] and self:is_valid_move(direction) then
            return direction
        end
    end
    -- TODO: what about very last tile (all previous tiles visited already)?
    return nil
end

function PipeMaze:update_tile()
    local ln, col = self.piece.ln, self.piece.col
    local distance = self:get_distance(ln, col)
    if distance >= self.steps then
        self.steps = distance
    end
    -- Set BEFORE map is updated, else we get bad values!
    self.direction = self:get_possible_move()
    -- After a while, `steps` does not represent the correct number of values!
    self.map[ln][col] = math.min(self.steps, distance)
    self.visited[ln][col] = true
    return self
end

function PipeMaze:move_piece()
    -- Very last tile will have no valid moves, so explicitly reset ourselves
    if self.direction == nil then
        self.piece.ln, self.piece.col = self.start.ln, self.start.col
        self.steps = self.steps + 1
       return self 
    end
    local move_fn = self.move_piece_fns[self.direction]
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

function PipeMaze:is_valid_move(direction)
    local test_fn = self.move_test_fns[direction]
    return test_fn(self)
end

function PipeMaze:is_starting_tile()
    return self.piece.ln == self.start.ln and self.piece.col == self.start.col
end

------------------------------ PIPEMAZE METATABLE ------------------------------

function PipeMaze:tostring()
    local ln, col = self.piece.ln, self.piece.col
    local char = self.map[ln][col]
    local distance = self:get_distance(ln, col)
    local output = {
        string.format("{CURRENT}: '%s' (Ln %i, Col %i)", char, ln, col),
        string.format("{DISTANCE}: %i units from start.", distance),
        string.format("{MAXSTEPS}: %i", self.steps),
        string.format("{PREVMOVE}: %s", self.direction or "none"),
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
