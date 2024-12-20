package aoc

import "core:fmt"
import "core:os"
import "core:strings"

PART :: #config(PART, 1)

main :: proc() {
    filename := "day03/sample.txt" if len(os.args) == 1 else os.args[1]
    raw_bytes, ok := os.read_entire_file(filename, context.allocator)
    if !ok {
        fmt.eprintfln("Failed to read file %q!", filename)
        return
    }
    defer delete(raw_bytes)
    
    lexer := lexer_create(string(raw_bytes))
    for {
        token := lexer_scan_token(&lexer)
        fmt.printfln("%-12v : %q", token.type, token.lexeme)
        if strings.ends_with(token.lexeme, "mul") {
            // TODO
        }
        if token.type == .Eof {
            break
        }
    }
}
