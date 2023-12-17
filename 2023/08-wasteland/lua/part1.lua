require "wasteland"

local PROJECT_DIR = WORKSPACE_DIR .. convert_OSpath("/2023/08-wasteland/")
local FALLBACK = PROJECT_DIR .. "sample.txt"

---@param query str
---@param arrows Arrows
---@param network Network
local function get_steps(query, arrows, network)
    local steps = 0
    while query ~= "ZZZ" do
        local target = network.nodes[query][arrows.arrow]
        print(query, target)
        query = target
        steps = steps + 1
        arrows:update()
    end
    return steps
end

---@param argc int
---@param argv str[]
local function main(argc, argv)
    local lines = readfile((argc == 1 and argv[1]) or FALLBACK)
    -- Remove the first line as it's not needed when parsing for nodes
    local arrows = Arrows:new(table.remove(lines, 1):toarray())
    local network = Network:new(lines)
    print(get_steps("AAA", arrows, network))
    return 0
end

return main(#arg, arg)
