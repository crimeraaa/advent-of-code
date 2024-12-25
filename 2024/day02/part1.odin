package aoc

import "core:fmt"

/*
Note
--------
Determine how many 'safe' reports there are. A 'safe' report is:
* Levels are ALL increasing or ALL decreasing.
* Any 2 adjacent levels must differ from 1 to 3.
--------
 */
part1 :: proc(data: Data) {
    safe := 0
    for report in data.reports {
        index: Pair = {-1, -1}
        ok, error := report_check(report, &index)
        if ok {
            safe += 1
        }
        fmt.printfln("%v = %v (Error: %v @ level %v)", report, ok, error, index)
    }
    fmt.printfln("There are %i safe reports.", safe)
}

/*
Parameters
--------
out_index:
    C-style out-parameter. Will hold the 0-based index of the first level
    where we encountered an error.
--------
 */
report_check :: proc(report: Report, out_index: ^Pair = nil) -> (is_safe: bool, error: Error) {
    left, right := report[:len(report) - 1], report[1:]

    for start_value, start_index in left {
        start_order, _ := order_get(start_value, right[start_index])

        for index in start_index..<len(right) {
            if _, error = order_get(left[index], right[index], start_order); error != .None {
                if out_index != nil {
                    out_index^ = {index, index + 1}
                }
                return false, error
            }
        }
    }

    return true, .None
}
