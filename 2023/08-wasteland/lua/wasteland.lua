require "util/common"
require "util/math"
require "util/stdio"
require "util/string"
require "util/table"
require "util/prettyprint"
require "arrows"

---@class Node
---@field L str String of the node to lookup to the left.
---@field R str String of the node to lookup to the right.

---@class Network
---@field nodes { [str]:Node } Query w/ string (e.g. "AAA") then update `Arrow`.
Network = {}

---@param lines str[]
function Network:new(lines)
    ---@type Network
    local network = {nodes = {}}
    for _, line in ipairs(lines) do
        -- Primary key, e.g. `"AAA"` to query into `nodes["AAA"]`.
        local source = line:match("(%w+) =")
    
        -- Primary key's destination mappings for left and right, e.g. 
        --```lua 
        --      nodes["AAA"] = {L = "BBB", R = "CCC"}
        --``` 
        -- Need to escape the outer parentheses so we don't capture everything.
        local left, right = line:match("= %((%w+), (%w+)%)")
        network.nodes[source] = {['L'] = left, ['R'] = right}
    end
    return network
end
