require "wasteland"

local PROJECT_DIR = WORKSPACE_DIR .. convert_OSpath("/2023/08-wasteland/")
local FALLBACK = PROJECT_DIR .. "part2sample.txt"

-- Given the full line, e.g. "AAA = (BBB, CCC)", filter keys only ending w/ {char}.
---@param char str
local function make_line_ending_filter(char)
    local pattern = string.format("([%%w]-[%s]) =", char)
    ---@param line str
    return function(line)
        return line:match(pattern)
    end
end

---@param lines str[]
---@param filter_fn fun(line:str):bool
local function filter_lines(lines, filter_fn)
    return table.copy(table.filter(lines, filter_fn), filter_fn)
end


---@param query str
---@param arrows Arrows
---@param network Network
local function get_steps(query, arrows, network)
    local steps = 0
    while not query:find("^%w-Z$") do
        local target = network.nodes[query][arrows.arrow]
        -- printf("=> %s ", target)
        query = target
        steps = steps + 1
        arrows:update()
    end
    return steps, query
end

---@param arrows Arrows
---@param network Network
---@param current str[]
local function get_all_steps(arrows, network, current)
    local steps = {} ---@type num[] collection of steps taken per starting node
    for ii = 1, #current do
        local query = current[ii]
        local count, found = get_steps(query, arrows, network)
        -- printf("\"%s\" => \"%s\": %i steps\n", query, found, count)
        steps[#steps + 1] = count
    end
    -- TODO: Get least common multiple
    -- e.g. steps[1] = 2, steps[2] = 3, LCM = 6
    for ii = 1, #steps do
        local number = steps[ii]
        local factors = math.factorize(number)
        printf("%s's factors: %s\n", number, table.concat(factors, ", "))
    end
    -- 18157 11653 21409 12737 14363 15989 => 9064949303801
    -- thanks python lmao
    return math.lcm(unpack(steps))
end

---@param argc int
---@param argv str[]
local function main(argc, argv)
    local lines = readfile((argc == 1 and argv[1]) or FALLBACK)
    -- Remove the first line as it's not needed when parsing for nodes
    local arrows = Arrows:new(table.remove(lines, 1):toarray())
    local network = Network:new(lines)

    -- Current nodes, start with keys ending in A only.
    ---@type str[]
    local current = filter_lines(lines, make_line_ending_filter('A'))
    local endings = filter_lines(lines, make_line_ending_filter('Z'))
    -- print_array(arrows.list, "arrows.list")
    print_table(current, "current")
    print_table(endings, "endings")
    printf("%.0f steps\n", get_all_steps(arrows, network, current))
    return 0
end

return main(#arg, arg)
