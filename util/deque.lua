require "util/array"
require "util/table"

-- Doubly-ended queue. Use the push and pop functions, index directly via [].
---@class Deque
---@field buffer any[]
deque = {}

-- Create a new doubly-ended queue for you to work with.
---@generic T
---@param ... T Values to start the internal buffer of with, or none.
---@return Deque
function deque.new(...)
    local dq = {
        buffer = {...} --- may start w/ empty buffer
    }
    return setmetatable(dq, deque.mt) 
end

-- Insert an element to the leftmost index, a.k.a. index 1.
-- Note that this takes care of updating the queue accordingly.
---@generic T
---@param item T
function deque:push_left(item)
    table.insert(self.buffer, 1, item)
    return self
end

-- Insert an element a new leftmost index, a.k.a. the very last index + 1.
-- Note that this takes care of updating the queue accordingly.
---@generic T
---@param item T
function deque:push_right(item)
    table.insert(self.buffer, item)
    return self
end

-- Removes the leftmost element (index 1) and returns it to you.
-- Note that this takes care of updating the queue accordingly.
---@generic T
---@return T
function deque:pop_left()
    return table.remove(self.buffer, 1)
end

-- Removes the rightmost element (index `#self.buffer`) and returns it to you.
-- Note that this takes care of updating the queue accordingly.
---@generic T
---@return T
function deque:pop_right()
    return table.remove(self.buffer, #self.buffer)
end

-- Returns the number of elements of the internal buffer.
function deque:len()
    return #self.buffer
end

---@type metatable
deque.mt = {}

-- Controls simple inheritance and somewhat "overloads" bracket indexing.
-- so that you have immediate access to the buffer, e.g.
--      
--      local dq = deque.new("hi", "mom!")
--      print(dq[1]) --> output: "hi"
-- 
---@generic T
---@param inst Deque An instance- not the primary `deque` table.
---@param key int|str
---@return T|function
function deque.mt.__index(inst, key)
    if type(key) == "number" then
        return inst.buffer[key]
    else
        return deque[key]
    end
end

-- Converts your deque's buffer into a nice-ish looking string.
---@param inst Deque
function deque.mt.__tostring(inst)
    return table.tostring(inst.buffer)
end

-- -- Gets the length of the internal buffer instead.
-- -- Not available with Lua 5.1.
-- ---@param inst Deque
-- function deque.mt.__len(inst)
--     return #inst.buffer
-- end
