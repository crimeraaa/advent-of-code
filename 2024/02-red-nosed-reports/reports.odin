package aoc

import "core:fmt"
import "core:math"
import "core:os"
import "core:strings"
import "core:strconv"

PART :: #config(PART, 1)
    
Data :: struct {
    values: [dynamic]int,       // Pseudo 2D array
    reports: [dynamic]Report,   // Series of views into `values`
    text: string,
}

Report :: struct {
    values: []int,
    levels: int,
    line: string,
}

Order :: enum {
    Equal = 0,
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
    filename := "sample.txt" if len(os.args) <= 1 else os.args[1]
    raw_bytes, ok := os.read_entire_file(filename)
    if !ok {
        fmt.eprintfln("Failed to read file '%s'!", filename)
        return
    }
    defer delete(raw_bytes)
    
    data := data_create(string(raw_bytes))
    defer data_destroy(&data)
    data_fill(&data)
    // data_print_reports(data)
    
    when PART == 1 {
        part1(data)
    } else when PART == 2 {
        part2(data)
    } else {
        #panic("Unknown PART number. Must be one of: 1, 2")
    }
}

order_check :: proc(x, y: int) -> (order: Order, in_range: bool) {
    diff := x - y
    switch {
        case diff > 0:  order = .Decreasing
        case diff < 0:  order = .Increasing
        case:           order = .Equal
    }
    in_range = 1 <= abs(diff) && abs(diff) <= 3
    return
}

/* 
    Given: "1 2 3 4 5\n6 7 8 9 10\n11 12 13 14 15\n16 17 18 19 20\n21 22 23 24 25\n<EOF>"
    
    lines = 5
    spaces = 20
 */
data_create :: proc(text: string, allocator := context.allocator) -> Data {
    lines, spaces := strings.count(text, "\n"), strings.count(text, " ")
    return {
        values  = make([dynamic]int, 0, lines + spaces, allocator),
        reports = make([dynamic]Report, 0 ,lines, allocator),
        text    = text,
    }
}

data_destroy :: proc(data: ^Data) {
    delete(data.values)
    delete(data.reports)
}

data_fill :: proc(data: ^Data) {
    text := data.text    
    for line in strings.split_lines_iterator(&text) {
        start := line
        prev_n := len(data.values)
        for item in strings.split_by_byte_iterator(&start, ' ') {
            value, _ := strconv.parse_int(item)
            append(&data.values, value)
        }
        values := data.values[prev_n:len(data.values)]
        report: Report = {
            values = values,
            levels = len(values),
            line = line,
        }
        append(&data.reports, report)
    }
}

data_print_reports :: proc(data: Data) {
    fmt.println(data.values)
    for report, index in data.reports {
        fmt.printfln("[%i] = %v", index, report)
    }
}
