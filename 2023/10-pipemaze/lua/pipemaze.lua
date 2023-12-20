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
---@field map str[][] Tile data.
---@field visited bool[][] Keep track of which tiles were previously visited.
---@field start MapInfo Starting piece's coordinates. Mostly read-only.
---@field piece MapInfo Current piece's coordinates, Is mutable & modifiable.
---@field dimensions MapInfo How big the map is, assumes it's a rectangle.
PipeMaze = {}

---------------------------- PIPEMAZE LOOKUP TABLES ----------------------------

-- List of keys (pipe shapes), each states which directions can be connected.
PipeMaze.shapes = require("pipedatabase").shapes
PipeMaze.directions = require("pipedatabase").directions

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

function PipeMaze:get_shape(ln, col)
    local neighbors = self:get_neighbors(ln, col)
    local connected = self:get_connections(neighbors)
    -- Linear search for 100% match on all possible connections
    for key, shape in pairs(self.shapes) do
        if self:compare_pipes(connected, shape) == true then
            return key
        end
    end
    return '.'
end

---@param left PipeInfo
---@param right PipeInfo
function PipeMaze:compare_pipes(left, right)
    for direction in pairs(self.directions) do
        if left[direction] ~= right[direction] then
            return false
        end
    end
    return true
end

-- Get the allowed connections for this neighbor.
function PipeMaze:get_connections(neighbors)
    -- Does their caller's north neighbor allow for a connection  to its south?
    -- Remember this is in relation to the neighbor, not the caller.
    ---@type PipeInfo
    return {
        north = self.shapes[neighbors.north]["south"],
        south = self.shapes[neighbors.south]["north"],
        east  = self.shapes[neighbors.east]["west"],
        west  = self.shapes[neighbors.west]["east"],
    }    
end

function PipeMaze:get_neighbors(ln, col)
    local grid = {} ---@type PipeChars[][]
    for i = -1, 1 do
        local line = {}
        for ii = -1, 1 do
            local offset_ln = ln + i
            local offset_col = col + ii
            if self:is_valid_offset(offset_ln, offset_col) then
                line[#line + 1] = self.map[offset_ln][offset_col]
            end
        end
        grid[#grid+1] = line
    end
    -- Get only the north, west, east and south neighbor pipe characters
    return {
        north = grid[1][2],
        west  = grid[2][1],
        east  = grid[2][3],
        south = grid[3][2],
    }
end

------------------------------- PIPEMAZE HELPERS -------------------------------

function PipeMaze:is_valid_offset(ln, col)
    return self:is_in_bounds(ln, "ln") and self:is_in_bounds(col, "col")
end

function PipeMaze:is_in_bounds(index, axis)
    return (index >= 1) and (index <= self.dimensions[axis])
end

------------------------------ PIPEMAZE METATABLE ------------------------------

function PipeMaze:tostring()
    local ln, col = self.piece.ln, self.piece.col
    local char = self.map[ln][col]
    local output = {
        string.format("{CURRENT}: '%s' (Ln %i, Col %i)", char, ln, col),
        string.format("{VISITED}: %s", tostring(self.visited[ln][col]))
    }
    for _, line in ipairs(self.map) do
        output[#output+1] = table.concat(line, " ")
    end
    return table.concat(output, "\n")
end

PipeMaze.mt = {
    __index = PipeMaze,

    __tostring = PipeMaze.tostring,
}
