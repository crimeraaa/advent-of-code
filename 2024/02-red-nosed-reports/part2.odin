package aoc

import "core:fmt"
import "core:slice"

part2 :: proc(data: Data) {
    safe := 0

    for report in data.reports {
        index := -1
        ok, error := report_check(report, &index)
        // Doesn't work anyway!
        if !ok {
            prev := index
            ok, error = secondary_check(report, &index);
            if !ok {
                index = prev + 1
                ok, error = secondary_check(report, &index)
            }
        }
        if ok {
            safe += 1
        }
        fmt.printfln("%v = %v (Error: %v @ level %i)", report.values, ok, error, index)
    }
    fmt.printfln("There are %i safe reports.", safe)
}

@(private="file")
secondary_check :: proc(report: Report, index: ^int, allocator := context.allocator) -> (is_safe: bool, err: Error) {
    buffer := slice.clone_to_dynamic(report.values[:], allocator)
    defer delete(buffer)
    ordered_remove(&buffer, index^)
    fmt.printfln("\tsans [%v]: %v", index^, buffer[:])
    dummy: Report = {
        values = buffer[:],
        levels = len(buffer),
        line   = "",
    }
    return report_check(dummy, index)
}
