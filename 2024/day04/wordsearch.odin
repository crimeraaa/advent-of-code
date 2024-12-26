package aoc

import "core:fmt"

PART :: #config(PART, 1)

main :: proc() {
    frame := frame_make(#load("input.txt", string), context.allocator)
    defer frame_destroy(&frame)

    frame_print(&frame)


    when PART == 1 {
        fmt.printfln("%q occurs %v times.", "XMAS", part1(&frame))
    } else when PART == 2 {
        fmt.printfln("%q occurs %v times", "X-MAS", part2(&frame))
    } else {
        #panic("PART must be one of: 1, 2")
    }
}

part1 :: proc(frame: ^Frame) -> (count: int) {
    for char, index in frame.grid {
        // Can't possibly start "XMAS"?
        if char != 'X' {
            continue
        }
        // Try to find adjacent occurences of "MAS"
        pos := index_to_vector(index, frame)
        for dir in Direction {
            cur_pos := pos
            cell_move('M', &cur_pos, dir, frame) or_continue
            cell_move('A', &cur_pos, dir, frame) or_continue
            cell_move('S', &cur_pos, dir, frame) or_continue
            fmt.println(pos, "=>", cur_pos)
            count += 1
        }
    }
    return count
}

/*
Possibilities
--------
```
M.M     M.S     S.S     S.M
.A.     .A.     .A.     .A.
S.S     M.S     M.M     S.M
```
--------
 */
part2 :: proc(frame: ^Frame) -> (count: int) {
    for char, index in frame.grid {
        // We assume 'A' is always the reference point.
        if char != 'A' {
            continue
        }

        /*
        Given:

            [ 'M' ] [ '.' ] [ 'S' ]
            [ '.' ] [ 'A' ] [ '.' ]
            [ 'M' ] [ '.' ] [ 'S ']

        Our first iteration will be .Northwest. We successfully find 'M' there
        and 'S' at the complement, .Southeast. Later on, when our iteration is
        at .Southeast, we find 'S' there. Because our 'if' checks first for 'M'
        this does not pass so we do not duplicate the matches.
         */
        pos := index_to_vector(index, frame)
        matches := 0
        for dir in direction_diagonals {
            if cell_check('M', pos, dir, frame) && cell_check('S', pos, direction_complements[dir], frame){
                matches += 1
            }
        }

        if matches == 2 {
            fmt.printfln("%q @ %v", "X-MAS", pos)
            count += 1
        }
    }
    return count
}

cell_check :: proc(expected: rune, vec: Vector, dir: Direction, frame: ^Frame) -> bool {
    r, ok := frame_at(frame, vec + direction_vectors[dir])
    return ok && r == expected
}

cell_move :: proc(expected: rune, cur_pos: ^Vector, dir: Direction, frame: ^Frame) -> bool {
    vector_update(cur_pos, dir, frame) or_return
    r, ok := frame_at(frame, cur_pos^)
    return ok && r == expected
}
