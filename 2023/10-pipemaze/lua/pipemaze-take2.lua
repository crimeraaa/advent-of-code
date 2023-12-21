require "util/common"
require "util/array"
require "util/stdio"
require "util/table"
require "util/prettyprint"

PROJECT_DIR = WORKSPACE_DIR .. convert_OSpath("/2023/10-pipemaze/")
FALLBACK = PROJECT_DIR .. "complex.txt"

---@alias TileChars '|'|'-'|'L'|'J'|'7'|'F'|'.'|'S'
---@alias NewDict<K, V> { [K]: V} 

---@class Maze 
---@field map str[][] 
---@field visited bool[][]

---@class Neighbor 
---@field ln -1|0|1
---@field col -1|0|1

---@class Location
---@field ln int
---@field col int
