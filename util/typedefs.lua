---@alias int integer
---@alias num number Or maybe we should use float? flt?
---@alias str string
---@alias bool boolean
---@alias tbl table
-- `fun` is already used as parameter functions so I think that's ok

---@class Array<T> : { [int]:T }
-- Think of using this like a C++ template.
-- For samples, see: https://luals.github.io/wiki/annotations/#generic
