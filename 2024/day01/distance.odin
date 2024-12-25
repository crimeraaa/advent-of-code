package aoc

import "core:fmt"
import "core:mem"
import "core:os"
import "core:slice"
import "core:strings"
import "core:strconv"

import "core:sys/posix"

PART  :: #config(PART, 1)
BASE  :: 10
Lists :: [2][dynamic]int

main :: proc() {
    // https://gist.github.com/karl-zylinski/4ccf438337123e7c8994df3b03604e33
    when ODIN_DEBUG {
        track: mem.Tracking_Allocator
        mem.tracking_allocator_init(&track, context.allocator)
        context.allocator = mem.tracking_allocator(&track)
        defer {
            check_allocations(track)
            mem.tracking_allocator_destroy(&track)
        }
    }

    buf: [256]byte
    fmt.println("CWD:\n", posix.getcwd(raw_data(buf[:]), len(buf)))

    lists := read_file_to_lists("day01/sample.txt" if len(os.args) <= 1 else os.args[1])
    defer {
        delete(lists.x)
        delete(lists.y)
    }
    when PART == 1 {
        part1(&lists)
    } else when PART == 2 {
        part2(&lists)
    } else {
        #panic("unknown PART number. Must be one of: 1, 2")
    }
}

read_file_to_lists :: proc(file_name: string, allocator := context.allocator) -> Lists {
    context.allocator = allocator
    fmt.println("READING:\n", file_name, sep = "")
    raw_data, ok := os.read_entire_file_from_filename(file_name)
    // Could not load file?
    if !ok {
        return nil
    }
    defer delete(raw_data)

    // Treat raw bytes as UTF-8 string (human-readable)
    data := string(raw_data)
    count := strings.count(data, "\n")
    lists: Lists

    // 0 initial length so we can append nicely, but non-zero backing capacity.
    lists.x = make([dynamic]int, 0, count)
    lists.y = make([dynamic]int, 0, count)

    for line in strings.split_lines_iterator(&data) {
        // output length parameter of parsed left number
        n: int
        left, _ := strconv.parse_int(line, BASE, &n)
        // Parse the original line sans left number
        right, _ := strconv.parse_int(strings.trim_left_space(line[n:]), BASE)
        append(&lists.x, left)
        append(&lists.y, right)
    }
    return lists
}

/*

// Same as 'math.sum()'
sum :: proc(values: []int) -> int {
    total := 0
    for value in values {
        total += value
    }
    return total
}
 */

check_allocations :: proc(track: mem.Tracking_Allocator) {
    if len(track.allocation_map) > 0 {
        fmt.eprintf("=== %v allocations not freed: ===\n", len(track.allocation_map))
        for _, entry in track.allocation_map {
            fmt.eprintf("- %v bytes @ %v\n", entry.size, entry.location)
        }
    }
    if len(track.bad_free_array) > 0 {
        fmt.eprintf("=== %v incorrect frees: ===\n", len(track.bad_free_array))
        for entry in track.bad_free_array {
            fmt.eprintf("- %p @ %v\n", entry.memory, entry.location)
        }
    }
}
