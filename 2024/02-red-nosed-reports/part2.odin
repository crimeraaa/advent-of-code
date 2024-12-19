package aoc

import "core:fmt"

part2 :: proc(data: Data) {
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
        error: Error
        switch order, in_range := order_check(x, right[i]); {
            case order == .Equal:       error = .Equal_Items
            case !in_range:             error = .Large_Differences
            case order != start_order:  error = .Unequal_Orders
            case:                       error = .None
        }
        /*
            TODO(2024-12-19):
                When we do encounter an error, we need to track the index.
                We can then use this to attempt to conceptually "remove" the item
                and check again to see if the report would still be 'safe'.
        */
        if error != .None {
            
        }
    }
    return true, .None
}
