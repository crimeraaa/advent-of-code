package aoc

import "core:fmt"
import "core:strconv"
import "core:strings"

Parser :: struct {
    lexer: Lexer,
    consumed, lookahead: Token,
}

parser_create :: proc(memory: string) -> (parser: Parser) {
    parser.lexer     = lexer_create(memory)
    parser.consumed  = {}
    parser.lookahead = {}
    return
}

parser_print :: proc(parser: Parser) {
    fmt.println(parser.consumed, ", ", parser.lookahead)
}

parser_advance :: proc(parser: ^Parser) {
    parser.consumed  = parser.lookahead
    parser.lookahead = lexer_scan_token(&parser.lexer)
}

parser_check_token :: proc(parser: ^Parser, type: Token_Type) -> (found: bool) {
    found = (parser.lookahead.type == type)
    return
}

parser_match_token :: proc(parser: ^Parser, type: Token_Type) -> (found: bool) {
    if found = parser_check_token(parser, type); found {
        parser_advance(parser)
    }
    return
}

parser_parse_number :: proc(parser: ^Parser) -> (value: int, ok: bool) {
    if !parser_match_token(parser, .Number) {
        return
    }
    return strconv.parse_int(parser.consumed.lexeme)
}

parser_parse_mul :: proc(parser: ^Parser) -> (product: int, ok: bool) {
    if !parser_match_token(parser, .Mul) {
        return
    }
    parser_match_token(parser, .Left_Paren)  or_return
    x := parser_parse_number(parser)         or_return
    parser_match_token(parser, .Comma)       or_return
    y := parser_parse_number(parser)         or_return
    parser_match_token(parser, .Right_Paren) or_return
    product, ok = x * y, true
    return
}

parser_parse_do_dont :: proc(parser: ^Parser) -> (enabled, ok: bool) {
    switch {
        case  parser_match_token(parser, .Do):   enabled = true
        case  parser_match_token(parser, .Dont): enabled = false
        case: return
    }
    // no args: "do()" or "don't()", no in between!
    parser_match_token(parser, .Left_Paren)  or_return
    parser_match_token(parser, .Right_Paren) or_return
    ok = true
    return
}
