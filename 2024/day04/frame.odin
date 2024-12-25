package aoc

import "core:fmt"
import "core:strings"

Frame :: struct {
    builder: strings.Builder, // Modified (allocated) view into primary input
    grid: string,             // View into `builder`
    rows, cols: int,          // Access control into `grid`
}

frame_make :: proc(input: string, allocator := context.allocator) -> (frame: Frame) {
    // Assumes we have 1 last blank newline as well.
    line_count := strings.count(input, "\n")

    // #chars per line, assumed to be consistent.
    line_length := strings.index_rune(input, '\n')

    // #chars in total, sans newlines.
    total_count := strings.rune_count(input) - line_count

    // Unnecessary but helps us support UTF-8 characters (in theory)
    builder, mem_err := strings.builder_make(0, total_count, allocator)

    // Unrecoverable
    assert(mem_err == nil)

    for char in input {
        if char != '\n' {
            strings.write_rune(&builder, char)
        }
    }

    frame = {
        builder = builder,
        grid = strings.to_string(builder),
        rows = line_count,
        cols = line_length,
    }

    return frame
}

frame_destroy :: proc(frame: ^Frame) {
    // Sets `frame.builder` to `nil`
    strings.builder_destroy(&frame.builder)
    frame.grid = ""
    frame.rows = 0
    frame.cols = 0
}

frame_print :: proc(frame: ^Frame) {
    fmt.printfln("[rows = %v, cols = %v]\n", frame.rows, frame.cols)
    fmt.printf("     ")
    for col in 0..<frame.cols {
        fmt.printf("[ %v ]", col)
    }
    // fmt.printf("\n[ 0 ]")
    for cell, index in frame.grid {
        // Account wrap-around (NOTE: Odin modulo precedence differs from C!)
        if index % frame.cols == 0 {
            fmt.printf("\n[ %v ]", index / frame.cols)
        }
        fmt.printf("[ %v ]", cell)
    }
    fmt.println('\n')
}

frame_at :: proc(frame: ^Frame, pos: Vector) -> (cell: rune, ok: bool) {
    if i := vector_to_index(pos, frame); 0 <= i && i < len(frame.grid) {
        return cast(rune)frame.grid[i], true
    }
    return rune(0), false
}
