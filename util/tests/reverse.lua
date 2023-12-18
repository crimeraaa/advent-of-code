require "util/array"
require "util/prettyprint"

local RANGE = 32

math.randomseed(os.time())
local t = {}
for i = 1, math.random(RANGE) do
    t[i] = math.random(RANGE)
end
-- print_array(t, "t")
-- print_array(array.reverse(t), "t")

print(table.concat(t, ", "))
print(table.concat(array.reverse(t), ", ")) 
-- tables to "forward" declare functions works!

local x = math.random(RANGE)
local found = array.search(t, math.random(RANGE))
if found then
    print(x .. " is in the array!")
else
    print(x .. " is not in the array.")
end
