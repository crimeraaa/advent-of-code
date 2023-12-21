require "util/deque"
require "util/prettyprint"

local points = deque.new({{1, 2}, {2, 3}}, {{3, 4}, {4, 5}})
print(points)
print("Popped left:", points:pop_left())
print(points)
local names = deque.new("Alice", "Bob", "Charlie")
print(names)
