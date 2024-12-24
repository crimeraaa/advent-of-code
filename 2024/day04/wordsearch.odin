package aoc

import "core:fmt"
import "core:strings"

PART  :: #config(PART, 1)
INPUT :: #load("sample.txt", string)

Vector :: distinct [2]int

Direction :: enum {
    North,
    South,
    East,
    West,
}   

/* 
Brief
--------
 *  Use these constant vectors to perform matrix math at a higher level.
--------

Note
--------
*   Our word search is conceptually arranged such that [0, 0] is the top left.
```
    x   [0]    [1]    [2]    ...    [m]
 y      
[0]     [0, 0] [1, 0] [2, 0]
[1]     [0, 1] [1, 1] [2, 1]
[2]     [0, 2] [1, 2] [2, 2]
...
[n]     [0, n] [1, n] [2, n]        [m, n]
```
--------
 */
Direction_Vectors :: [Direction]Vector {
    .North = { 0, -1},
    .East  = {+1,  0},
    .South = { 0, +1},
    .West  = {-1,  0},
}

Direction_Set :: bit_set[Direction]

main :: proc() {
    lines := make([dynamic]string, 0, strings.count(INPUT, "\n"), context.allocator)
    defer delete(lines)

    dummy := INPUT
    for line in strings.split_lines_iterator(&dummy) {
        append(&lines, line)
    }
    
    for line, number in lines {
        fmt.printfln("%v: %v", number, line)
    }
}
