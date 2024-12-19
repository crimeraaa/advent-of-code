package aoc

import "core:fmt"

/* 
   Determine how many 'safe' reports there are. A 'safe' report is:

   - Levels are ALL increasing or ALL decreasing.
   - Any 2 adjacent levels must differ from 1 to 3.
 */
part1 :: proc(data: Data) {
    safe := 0
    for report in data.reports {
        ok, Error := report_check(report)
        if ok {
            safe += 1
        }
        fmt.printfln("%v = %v (Error: %v)", report.values, ok, Error)
    }
    fmt.printfln("There are %i safe reports.", safe)
}

@(private="file")
report_check :: proc(report: Report) -> (is_safe: bool, error: Error) {
    left, right  := report.values[:report.levels - 1], report.values[1:]
    start_order, _ := order_check(left[0], right[0])

    compare_loop: for x, i in left {
        switch order, in_range := order_check(x, right[i]); {
            case order == .Equal:       return false, .Equal_Items
            case !in_range:             return false, .Large_Differences
            case order != start_order:  return false, .Unequal_Orders
        }
    }
    return true, .None
}
