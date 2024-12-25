package aoc

import "core:fmt"
import "core:math"
import "core:os"
import "core:strings"
import "core:strconv"
import "core:mem"

PART :: #config(PART, 1)
Pair :: distinct [2]int
Data :: struct {
    values: [dynamic]int,       // Pseudo 2D array
    reports: [dynamic]Report,   // Series of views into `values`
}

Report :: []int

Order :: enum {
    None = 0,
    Equal,
    Increasing,
    Decreasing,
}

Error :: enum {
    None = 0,
    Equal_Items,
    Unequal_Orders,
    Large_Differences,
}

main :: proc() {
    // https://gist.github.com/karl-zylinski/4ccf438337123e7c8994df3b03604e33
    when ODIN_DEBUG {
        track: mem.Tracking_Allocator
        mem.tracking_allocator_init(&track, context.allocator)
        context.allocator = mem.tracking_allocator(&track)

        defer {
            if n := len(track.allocation_map); n > 0 {
                fmt.eprintfln("=== %v allocations not freed: ===", n)
                for _, entry in track.allocation_map {
                    fmt.eprintfln("- %v bytes @ %v", entry.size, entry.location)
                }
            }
            if n := len(track.bad_free_array); n > 0 {
                fmt.eprintfln("=== %v incorrect frees: ===", n)
                for entry in track.bad_free_array {
                    fmt.eprintfln("- %p @ %v", entry.memory, entry.location)
                }
            }
            mem.tracking_allocator_destroy(&track)
        }
    }

    filename := "day02/sample.txt" if len(os.args) <= 1 else os.args[1]
    raw_bytes, ok := os.read_entire_file(filename)
    if !ok {
        fmt.eprintfln("Failed to read file '%s'!", filename)
        return
    }
    defer delete(raw_bytes)

    data := data_create(string(raw_bytes))
    defer data_destroy(&data)

    when PART == 1 {
        part1(data)
    } else when PART == 2 {
        part2(data)
    } else {
        #panic("Unknown PART number. Must be one of: 1, 2")
    }
}

order_get :: proc(x, y: int, prev := Order.None) -> (order: Order, error: Error) {
    diff := x - y
    in_range := 1 <= abs(diff) && abs(diff) <= 3
    switch {
        case diff > 0: order = .Decreasing
        case diff < 0: order = .Increasing
        case:          order = .Equal
    }
    switch {
        case order == .Equal, prev == .Equal:   error = .Equal_Items
        case order != prev:                     error = .Unequal_Orders
        case !in_range:                         error = .Large_Differences
        case:
    }
    return
}

/*
    Given: "1 2 3 4 5\n6 7 8 9 10\n11 12 13 14 15\n16 17 18 19 20\n21 22 23 24 25\n<EOF>"

    lines = 5
    spaces = 20
 */
data_create :: proc(text: string, allocator := context.allocator) -> (data: Data) {
    lines, spaces := strings.count(text, "\n"), strings.count(text, " ")
    data = {
        values  = make([dynamic]int, 0, lines + spaces, allocator),
        reports = make([dynamic]Report, 0 ,lines, allocator),
    }
    iterator := text
    for line in strings.split_lines_iterator(&iterator) {
        start := line
        prev_n := len(data.values) // save here as 'len()' updates via 'append()'
        for item in strings.split_by_byte_iterator(&start, ' ') {
            value, _ := strconv.parse_int(item)
            append(&data.values, value)
        }
        append(&data.reports, data.values[prev_n:len(data.values)])
    }
    return data
}

data_destroy :: proc(data: ^Data) {
    delete(data.values)
    delete(data.reports)
}

data_print_reports :: proc(data: Data) {
    fmt.println(data.values)
    for report, index in data.reports {
        fmt.printfln("[%i] = %v", index, report)
    }
}
