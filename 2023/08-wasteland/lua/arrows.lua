---@class Arrows
---@field list ('L'|'R')[] List of directions to take in a particular order.
---@field arrow 'L'|'R' Current direction so we can update `index` separately.
---@field length int #elements in `list`, controls when to reset `index`.
---@field index int Direct index into `list`, reset to 1 when exceeds length.
Arrows = {}

-- Implemeneted near the bottom of the file due to procedural nature of Lua.
-- 
-- `Arrows.mt.__tostring = Arrows.tostring` results in `nil`.
-- 
-- Cuz `Arrows:tostring` is not defined yet at this point!
-- 
-- Tables, by their nature as references/pointers, let us "forward" declare them,
-- so "forward" declaring the metatable works.
Arrows.mt = {}

-- Initialize a new directions database.
---@param chararray ('L'|'R')[]
function Arrows:new(chararray)
    ---@type Arrows
    local inst = {
        list = chararray, 
        arrow = chararray[1],
        length = #chararray, 
        index = 1
    }
    return setmetatable(inst, Arrows.mt)
end

-- Increment our index or reset, then update the current character.
-- If index exceeds length, index is reset to 1 so we can wrap around.
function Arrows:update()
    if self.index >= self.length then
        self.index = 1 -- Adding 1 would exceed length, so reset it.
    else
        self.index = self.index + 1
    end
    self.arrow = self.list[self.index]
    return self
end

-- If caller's key doesn't exist, fallback total `Arrows` table's keys.
Arrows.mt.__index = function(tbl, key)
    return Arrows[key]
end

-- Implicitly convert our table for use in functions like `print`.
---@param self Arrows
Arrows.mt.__tostring = function(self)
    return string.format("Index: %i, Direction: %s", self.index, self.arrow)
end
