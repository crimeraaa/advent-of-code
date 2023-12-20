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

local direction_lookup = {
    ["ln"] = function(offset) ---@param offset -1|1
        return (offset == -1 and "north") or "south"
    end,
    ["col"] = function(offset) ---@param offset -1|1
        return (offset == -1 and "west") or "east"
    end,
}

local visited_lookup = {
    ["ln"] = function(self, index) ---@param self PipeMaze
        return self.visited[index][self.piece.col]
    end,
    ["col"] = function(self, index) ---@param self PipeMaze
        return self.visited[self.piece.ln][index]
    end
}

---@param axis "ln"|"col"
---@param offset -1|1
local function make_move_fn(axis, offset)
    local visited_already = visited_lookup[axis]
    return function(self) ---@param self PipeMaze
        local test = self.piece[axis] + offset
        local in_range = self:is_in_bounds(test, "ln")
        if in_range and not visited_already(self, test) then
            self.piece[axis] = test
        end
        return self
    end
end

database.move_piece_fns = {
    north = make_move_fn("ln", -1), -- to go upwards, decrement y-axis
    west  = make_move_fn("col", -1),
    east  = make_move_fn("col", 1),
    south = make_move_fn("ln", 1), -- to go downwards, increment y-axis
}

return database
