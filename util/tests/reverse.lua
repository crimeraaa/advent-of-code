require "util/array"
require "util/prettyprint"

math.randomseed(os.time())
local t = {}
local limit = math.random(32)
for i = 1, limit do
    t[i] = math.random(32)
end
print_array(t, "t")
print_array(array.reverse(t), "t")
