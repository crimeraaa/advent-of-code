require "util/array"
require "util/stdio"
require "util/table"
require "util/prettyprint"

local t3, t4 = {"hi", "mom"}, {["hello"] = true, ["there"] = 15}
t3 = setmetatable(t3, {__index = t4})
t4 = setmetatable(t4, {__index = t3})
print(table.dump(t3, "t3"))
print(table.dump(t4, "t4"))
