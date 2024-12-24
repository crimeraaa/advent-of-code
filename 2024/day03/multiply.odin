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

    // consumed, lookahead = {}, {}
    parser := parser_create(string(raw_bytes))

    // consumed, lookahead = lookahead, scan_token()
    parser_advance(&parser)

    fmt.println(parser.lexer.data)
    
    result := 0
    for !parser_check_token(&parser, .Eof) {
        token_print(parser.lookahead)

        // Keep going until we probably have a preamble: "mul"
        if !parser_match_token(&parser, .Mul) {
            // parser_print(parser)
            parser_advance(&parser)
            continue
        }
        
        // '(' <number> ',' <number> ')'
        parser_match_token(&parser, .Left_Paren)  or_continue

        args: [2]int
        args.x = parser_parse_number(&parser)     or_continue
        parser_match_token(&parser, .Comma)       or_continue
        args.y = parser_parse_number(&parser)     or_continue
        parser_match_token(&parser, .Right_Paren) or_continue
        
        // fmt.printfln("\tmul(%v, %v)", args.x, args.y)
        result += args.x * args.y
    }
    fmt.printfln("Result: %v", result)
}
