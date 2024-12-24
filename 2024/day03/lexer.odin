package aoc

import "core:strings"

Lexer :: struct {
    data: string,
    start, stop, end: int,
}

lexer_create :: proc(memory: string) -> Lexer {
    return {data = memory, start = 0, stop = 0, end = len(memory) - 1}
}

lexer_scan_token :: proc(lexer: ^Lexer) -> Token {
    if is_at_end(lexer) {
        return token_create(lexer, .Eof)
    }
    r := peek_current(lexer)
    
    if consume_sequence(lexer, is_alpha) > 0 {
        token := token_create(lexer, .Invalid)
        switch {
            case strings.has_suffix(token.lexeme, "mul"):   token.type = .Mul
            case strings.has_suffix(token.lexeme, "do"):    token.type = .Do
            case strings.has_suffix(token.lexeme, "don't"): token.type = .Dont
            case:
        }
        return token
    } else if consume_sequence(lexer, is_digit) > 0 {
        return token_create(lexer, .Number)
    }
    
    advance_current(lexer)
    switch r {
        case '(': return token_create(lexer, .Left_Paren)
        case ')': return token_create(lexer, .Right_Paren)
        case ',': return token_create(lexer, .Comma)
        case:
            consume_sequence(lexer, is_not_alnum)
            return token_create(lexer, .Invalid)
    }
}

@private
is_alpha :: proc(r: rune) -> bool {
    return ('a' <= r && r <= 'z') || ('A' <= r && r <= 'Z') || (r == '_') || (r == '\'')
}

@private
is_digit :: proc(r: rune) -> bool {
    return '0' <= r && r <= '9'
}

@private
is_not_alnum :: proc(r: rune) -> bool {
    return !is_alpha(r) && !is_digit(r)
}

// pls inline
@private
consume_sequence :: proc(lexer: ^Lexer, $callback: proc(r: rune) -> bool) -> (count: int) {
    /* 
    Note (2024-12-24):
        We check for is_at_end() because otherwise, if we're in the middle of this loop AS
        we hit the end, we'd be stuck in an infinite loop.
     */
    for callback(peek_current(lexer)) && !is_at_end(lexer) {
        advance_current(lexer)
        count += 1
    }
    return
}

@private
token_create :: proc(lexer: ^Lexer, type: Token_Type) -> (token: Token) {
    token.lexeme = lexer.data[lexer.start:lexer.stop]
    token.type   = type
    lexer.start  = lexer.stop // Update for next token's lexeme to be valid
    return
}

/* 
Note
--------
* Remember that `lexer.end` is the 0-based last valid index of `lexer.data`.
--------
 */
@private
is_at_end :: proc(lexer: ^Lexer) -> bool {
    return lexer.stop >= lexer.end
}

@private
peek_current :: proc(lexer: ^Lexer) -> rune {
    return cast(rune)lexer.data[lexer.stop]
}

@private
advance_current :: proc(lexer: ^Lexer) {
    if is_at_end(lexer) {
        return
    }
    lexer.stop += 1
}

