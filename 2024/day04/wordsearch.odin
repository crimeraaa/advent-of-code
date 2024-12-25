package aoc

import "core:fmt"

PART :: #config(PART, 1)

main :: proc() {
    frame := frame_make(#load("input.txt", string), context.allocator)
    defer frame_destroy(&frame)

    frame_print(&frame)

    count := 0
    for char, index in frame.grid {
        // Can't possibly start "XMAS"?
        if char != 'X' {
            continue
        }
        cells: [4]rune
        cells[0] = char

        // Try to find adjacent occurences of "MAS"
        pos: Vector = index_to_vector(index, &frame)
        for dir in Direction {
            cur_pos := pos
            cells[1] = cell_try('M', &cur_pos, dir, &frame) or_continue
            cells[2] = cell_try('A', &cur_pos, dir, &frame) or_continue
            cells[3] = cell_try('S', &cur_pos, dir, &frame) or_continue
            fmt.println(pos, "=>", cur_pos)
            count += 1
        }
    }

    fmt.printfln("%q occurs %v times.", "XMAS", count)
}

cell_try :: proc(expected: rune, cur_pos: ^Vector, dir: Direction, frame: ^Frame) -> (cell: rune, ok: bool) {
    vector_update(cur_pos, dir, frame) or_return
    if r, ok2 := frame_at(frame, cur_pos^); ok2 {
        return r, r == expected
    }
    return 0, false
}
