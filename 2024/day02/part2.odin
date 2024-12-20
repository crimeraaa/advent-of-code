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
            case 1: // TODO
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

@(private="file")
secondary_check :: proc(report: Report, index: int, allocator := context.allocator) -> (is_safe: bool, err: Error) {
    buffer := slice.clone_to_dynamic(report[:], allocator)
    defer delete(buffer)
    ordered_remove(&buffer, index)
    fmt.printfln("\tsans [%v]: %v", index, buffer[:])
    return report_check(buffer[:], nil)
}