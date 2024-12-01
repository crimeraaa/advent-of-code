package aoc

import "core:fmt"
import "core:math"
import "core:slice"
import "core:strings"

part1 :: proc(lists: ^Lists, allocator := context.allocator) {
    context.allocator = allocator
    // fmt.printfln("[UNSORTED]:\n%v\n%v", lists.x, lists.y)

    // Quick sort, lowest to highest
    slice.sort(lists.x[:])
    slice.sort(lists.y[:])

    // fmt.printfln("[SORTED]:\n%v\n%v", lists.x, lists.y)
    
    diffs := make([dynamic]int, len(lists.x))
    defer delete(diffs)
    
    for left, index in lists.x {
        right := lists.y[index]
        diffs[index] = abs(left - right)
    }
    // fmt.printfln("[DIFFERENCES]:\n%v", diffs)
    fmt.printfln("[TOTAL DISTANCE]:\n%v", math.sum(diffs[:]))
}
