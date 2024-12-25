package aoc

import "core:fmt"
import "core:slice"
import "core:math"

part2 :: proc(lists: ^Lists, allocator := context.allocator) {
    context.allocator = allocator
    // fmt.printfln("[DATA]:\n%v\n%v", lists.x, lists.y)

    counted := make([dynamic]int, len(lists.x))
    defer delete(counted)

    /*
        Given:

        x = [3, 4, 2, 1, 3, 3]
        y = [4, 3, 5, 3, 9 ,3]

        Occurences of x in y:
        = [3, 1, 0, 0, 3, 3]

        Multiply occurences by corresponding x-value:
        = [3*3, 4*1, 2*0, 1*0, 3*3, 3*3]
        = [9, 4, 0, 0, 9, 9]

        Sum it up:
        = 9 + 4 + 0 + 0 + 9 + 9
        = 31
     */
    for value, index in lists.x {
        counted[index] = slice.count(lists.y[:], value) * value
    }

    fmt.printfln("[SIMILARITY]:\n%v", math.sum(counted[:]))
}

/*
// Exactly the same as 'slice.count'
count_occurences :: proc(needle: int, haystack: []int) -> int {
    count := 0
    for value in haystack {
        if value == needle {
            count += 1
        }
    }
    return count
}
 */
