package aoc

import "core:fmt"
import "core:os"
import "core:strings"

PART  :: #config(PART, 2)
INPUT :: #load("input.txt", string)

#assert(1 <= PART && PART <= 2)

main :: proc() {
    // consumed, lookahead = {}, {}
    parser := parser_create(INPUT)

    // consumed, lookahead = lookahead, scan_token()
    parser_advance(&parser)

    fmt.println("PART:", PART, parser.lexer.data)
    fmt.printfln("Result: %v", part1(&parser) when PART == 1 else part2(&parser))
}

part1 :: proc(parser: ^Parser) -> (result: int) {
    for !parser_check_token(parser, .Eof) {
        token_print(parser.lookahead)

        // fmt.printfln("\tmul(%v, %v)", args.x, args.y)
        product, ok := parser_parse_mul(parser)
        if ok {
            fmt.printfln("result = %v + %v", result, product)
            result += product
        } else {
            parser_advance(parser)
        }
    }
    return
}

part2 :: proc(parser: ^Parser) -> (result: int) {
    enabled := true
    for !parser_check_token(parser, .Eof) {
        token_print(parser.lookahead)

        product, ok := parser_parse_mul(parser)
        if ok {
            if enabled {
                fmt.printfln("\tresult = %v + %v", result, product)
                result += product
            }
            continue
        }
        tmp: bool
        tmp, ok = parser_parse_do_dont(parser)
        if ok {
            fmt.printfln("\tmul %s", "enabled" if tmp else "disabled")
            enabled = tmp
        } else {
            parser_advance(parser)
        }
    }
    return
}
