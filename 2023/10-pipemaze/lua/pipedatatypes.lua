---@meta

---@alias PipeChars int|'|'|'-'|'L'|'J'|'7'|'F'|'.'|'S'
---@alias DirectionStrs "north"|"west"|"east"|"south"

---@class MapInfo
---@field ln int Line number (or number of lines in total).
---@field col int Column number, index into `lines` (or line length).

---@class PipeInfo
---@field north bool Pipe-shape allows/disallows connections above.
---@field west bool Pipe-shape allows/disallows connections to its left.
---@field east bool Pipe-shape allows/disallows connections to its right.
---@field south bool Pipe-shape allows/disallows connections below.

---@alias NeighborSet { [str]: MapInfo }
