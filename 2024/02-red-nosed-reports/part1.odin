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
        ok, error := report_check(report)
        if ok {
            safe += 1
        }
        fmt.printfln("%v = %v (Error: %v)", report.values, ok, error)
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
report_check :: proc(report: Report, out_index: ^int = nil) -> (is_safe: bool, error: Error) {
    left, right    := report.values[:report.levels - 1], report.values[1:]
    start_order, _ := order_check(left[0], right[0])

    // fmt.print("\tcomparing: ")
    for x, i in left {
        // fmt.printfln("\t%v: [%v, %v]", i, x, right[i])
        switch order, in_range := order_check(x, right[i]); {
            case order == .Equal:       error = .Equal_Items
            case !in_range:             error = .Large_Differences
            case order != start_order:  error = .Unequal_Orders
        }
        if error != .None {
            if out_index != nil {
                out_index^ = i
            }
            return false, error
        }
    }
    // fmt.println()
    return true, .None
}
