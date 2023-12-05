local fp = assert(io.open(arg[1] or "./input.txt", "r"))
local count = 1
local sum = 0

---@param fmt string
---@param ... string|number
local function printf(fmt, ...)
    io.stdout:write(fmt:format(...))
end

---@param line string
local function find_numbers(line)
    -- start of string capture nearest digit, non-greedy skip non-digits before us
    local _, _, left = line:find("^[%D]-(%d)")
    -- end of string capture nearest digit, non-greedy skip non-digits after us
    local _, _, right = line:find("(%d)[%D]-$")
    return left, right
end

for line in fp:lines("*l") do
    if (line ~= "") then 
        local left, right = find_numbers(line)
        printf("%i : '%s', Digits: %i, %i\n", count, line, left, right)
        count = count + 1
        -- We want to treat 1 and 2 as 12, 7 and 7 as 77, etc.
        sum = sum + (left * 10) + right;
    end
end
printf("Sum: %i\n", sum)

fp:close()
