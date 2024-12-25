package aoc

import "core:fmt"
import "core:slice"

/*
Description
--------
When we encounter a Report with an error, we need to determine which 1 offending
level can be removed and result in the report being 'safe'.
--------
 */
part2 :: proc(data: Data) {
    safe := 0

    for report in data.reports {
        index := Pair{-1, -1}
        count := report_count_errors(report, &index)
        switch count {
            case 0: safe += 1
            case 1:
                buffer := slice.clone_to_dynamic(report[:], context.allocator)
                defer delete(buffer)

                // Restore later when trying the 2nd option
                tmp := buffer[index.y]
                ordered_remove(&buffer, index.y)
                fmt.printfln("\tTry: %v", buffer)
                if report_count_errors(buffer[:]) == 0 {
                    safe += 1
                    count -= 1
                } else {
                    buffer[index.x] = tmp
                    fmt.printfln("\tTry: %v", buffer)
                    if report_count_errors(buffer[:]) == 0 {
                        safe += 1
                        count -= 1
                    }
                }
            case:   // We can only remove 1 level, this will always be unsafe.
        }
        fmt.printfln("%v = (%v errors, first @ %v)", report, count, index)
    }
    fmt.printfln("There are %i safe reports.", safe)
}

/*
Parameters
--------
out_index:
    If non-'nil', will contain the indices of the first pair where we encountered
    an error.
--------
 */
report_count_errors :: proc(report: Report, out_index: ^Pair = nil) -> (count: int) {
    left, right    := report[:len(report) - 1], report[1:]
    start_order, _ := order_get(left[0], right[0])

    noted := false
    for value, index in right {
        if _, error := order_get(left[index], value, start_order); error != .None {
            if !noted {
                noted = true
                if out_index != nil {
                    out_index^ = {index, index + 1}
                }
            }
            count += 1
        }
    }
    return count
}
