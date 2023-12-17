require "util/common"
require "util/math"
require "util/stdio"
require "util/string"
require "util/table"
require "util/prettyprint"

-- This is a test to get the least common multiple of variable numbers.
---@param argc int
---@param argv str[]
local function main(argc, argv)
    local values = {}
    if argc >= 2 then
        for i, argument in ipairs(argv) do
            values[#values + 1] = tonumber(argument)
        end
    else
        values = {18157, 11653, 21409, 12737, 14363, 15989}
        -- values = {2, 3, 5}
    end
    print(math.lcm(unpack(values)))
    return 0
end

return main(#arg, arg)
