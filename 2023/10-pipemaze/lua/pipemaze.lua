require "util/common"
require "util/array"
require "util/math"
require "util/stdio"
require "util/string"
require "util/table"
require "util/prettyprint"

PROJECT_DIR = WORKSPACE_DIR .. convert_OSpath("/2023/10-pipemaze/")
FALLBACK = PROJECT_DIR .. "sample.txt"

---@class MazeCoord
---@field ln int Line number.
---@field col int Index into line `ln`.

---@class PipeMaze
---@field tiles str[][] Array of char arrays.
---@field piece MazeCoord Location of 'S' char in the maze.
PipeMaze = {}

---@param lines str[]
function PipeMaze:new(lines)
    -- Upvalue line counter so callback function can correctly set `piece`.
    local line_number = 0

    ---@type MazeCoord
    local piece = {col=0, ln=0} 

    ---@type str[][]
    local tiles = table.copy(lines, function(v)
        line_number = line_number + 1
        local line_column = string.find(v, 'S')
        if line_column then
            piece.ln, piece.col = line_number, line_column
        end
        return string.toarray(v) 
    end)

    ---@type PipeMaze
    return {piece = piece, tiles = tiles}
end

--[[ 
The pipes are arranged in a two-dimensional grid of tiles:

    | is a vertical pipe connecting north and south.
    - is a horizontal pipe connecting east and west.
    L is a 90-degree bend connecting north and east.
    J is a 90-degree bend connecting north and west.
    7 is a 90-degree bend connecting south and west.
    F is a 90-degree bend connecting south and east.
    . is ground; there is no pipe in this tile.
    S is the starting position of the animal; there is a pipe on this tile, but your sketch doesn't show what shape the pipe has.
]]
