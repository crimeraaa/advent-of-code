require "util/common"
require "util/array"
require "util/math"
require "util/stdio"
require "util/string"
require "util/table"
require "util/prettyprint"

PROJECT_DIR = WORKSPACE_DIR .. convert_OSpath("/2023/09-mirage/")
FALLBACK = PROJECT_DIR .. "sample.txt"

-- Determine if `sequence` is composed of all `0` values. If not, we'll recurse.
---@param sequence int[]
function is_empty_sequence(sequence)
    for _, diff in ipairs(sequence) do
        if diff ~= 0 then
            return false
        end
    end
    return true
end

---@param sequence int[] Current integer array sequence to list differences of.
---@param collection? int[][]
---@nodiscard
function get_history(sequence, collection)
    -- Create collection w/ `sequence` at the front so caller doesn't have to.
    collection = collection or {[1] = sequence}

    -- Make the handle first so we don't have to type the table length operator.
    ---@type int[]
    local differences = {} 

    -- Make sure collection also has the handle so caller gets it back as well.
    collection[#collection + 1] = differences

    -- List the differences between all of sequence's contiguous pairs.
    for _, vx, _, vy in array.ipairs.two(sequence) do
        differences[#differences + 1] = vy - vx
    end

    if is_empty_sequence(differences) then
        return collection
    end
    -- Tail call recursion: extrapolate history until we reach a sequence that
    -- is composed entirely of zeroes.
    -- 
    -- This recursive call will continuously update `collection` until then.
    return get_history(differences, collection)
end
