local database = {}

database.shapes = {
    ---@type PipeInfo vertical pipe connecting north and south.
    -- Connects to either above or below caller.
    ['|'] = {
        north = true,  south = true, 
        east  = false, west  = false
    },
    ---@type PipeInfo horizontal pipe connecting east and west.
    -- Connects to either left or right of caller.
    ['-'] = {
        north = false, south = false, 
        east  = true,  west  = true,
    }, 
    ---@type PipeInfo 90-degree bend connecting north and east.
    -- Connects to either below caller or left of caller.
    ['L'] = {
        north = true, south = false, 
        east  = true, west  = false
    }, 
    ---@type PipeInfo 90-degree bend connecting north and west.
    -- Connects to either below caller or right of caller.
    ['J'] = {
        north = true,  south = false,
        east  = false, west  = true,
    }, 
    ---@type PipeInfo 90-degree bend connecting south and west.
    -- Connects to either above caller or right of caller.
    ['7'] = {
        north = false, south = true, 
        east  = false, west  = true,
    }, 
    ---@type PipeInfo 90-degree bend connecting south and east.
    -- Connects to either above caller or left of caller.
    ['F'] = {
        north = false, south = true,
        east  = true,  west  = false,
    }, 
    ---@type PipeInfo ground. there is no pipe in this tile.
    -- Does not connect to caller in any way because it's not a pipe.
    ['.'] = {
        north = false, south = false, 
        east  = false, west  = false
    }, 
    ---@type PipeInfo starting position of the animal; there's a pipe here
    -- but your sketch doesn't show what shape the pipe has!
    -- Does not connect to caller in any way because it's not a pipe.
    ['S'] = {
        north = false, south = false, 
        east  = false, west  = false
    },
}

-- Dictionary of directions and their complements/opposites.
database.directions = {
    ["north"] = "south", 
    ["west"] = "east", 
    ["east"] = "west", 
    ["south"] = "north",
}

---@param axis "ln"|"col"
---@param offset -1|1
local function make_move_fn(axis, offset) 
    return function(self) ---@param self PipeMaze
        local index = self.piece[axis] ---@type int
        local updated = index + offset ---@type int
        local in_range = self:is_in_bounds(updated, axis)
        if in_range then
            self.piece[axis] = updated
        end
        return self
    end
end

database.move_to = {
    north = make_move_fn("ln", -1), -- to go upwards, decrement y-axis
    west  = make_move_fn("col", 1),
    east  = make_move_fn("col", -1),
    south = make_move_fn("ln", 1), -- to go downwards, increment y-axis
}

database.test_offset = {
    north = function(self) ---@param self PipeMaze
        local testln = self.piece.ln
        local updated = testln - 1
        if self:is_in_bounds(updated, "ln") then
            testln = updated
        end
        return not self.visited[testln][self.piece.col]
    end,
    
    west = function(self) ---@param self PipeMaze
        local testcol = self.piece.col
        local updated = testcol - 1
        if self:is_in_bounds(updated, "col") then
            testcol = updated
        end
        return not self.visited[self.piece.ln][testcol]
    end,

    east = function(self) ---@param self PipeMaze
        local testcol = self.piece.col
        local updated = testcol + 1
        if self:is_in_bounds(updated, "col") then
            testcol = updated
        end
        return not self.visited[self.piece.ln][testcol]
    end,

    south = function(self) ---@param self PipeMaze
        local testln = self.piece.ln
        local updated = testln + 1
        if self:is_in_bounds(updated, "ln") then
            testln = updated
        end
        return not self.visited[testln][self.piece.col]
    end,
}

return database
