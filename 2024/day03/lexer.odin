package aoc

import "core:strings"
import "core:unicode"

Lexer :: struct {
    data, current: string,
    index: int,
}

Token :: struct {
    lexeme: string,
    type: Token_Type,
}

Token_Type :: enum {
    Invalid = 0,    // Anything that does not fit anything below.
    Word,           // A sequnce of the ASCII characters 'a' to 'z' or 'A' to 'Z'.
    Number,         // A sequence of the ASCII characters '0' to '9'.
    Left_Paren,     // The ASCII character '('.
    Right_Paren,    // The ASCII character ')'.
    Comma,          // The ASCII character ','.
    Eof,            // Not a character per se, acts more like a signal.
}

lexer_create :: proc(memory: string) -> (lexer: Lexer) {
    return {data = memory, current = memory, index = 0}
}

lexer_scan_token :: proc(lexer: ^Lexer) -> Token {
    if is_at_end(lexer) {
        return create_token(lexer, .Eof)
    }
    r := peek_current(lexer)
    if unicode.is_alpha(r) {
        advance_current(lexer)
        consume_sequence(lexer, unicode.is_alpha)
        return create_token(lexer, .Word)
    } else if unicode.is_digit(r) {
        advance_current(lexer)
        consume_sequence(lexer, unicode.is_digit)
        return create_token(lexer, .Number)
    }

    advance_current(lexer)
    switch r {
        case '(': return create_token(lexer, .Left_Paren)
        case ')': return create_token(lexer, .Right_Paren)
        case ',': return create_token(lexer, .Comma)
        case:
            consume_sequence(lexer, not_alpha_or_digit)
            return create_token(lexer, .Invalid)
    }
}

@private
not_alpha_or_digit :: proc(r: rune) -> bool {
    return !unicode.is_alpha(r) && !unicode.is_digit(r)
}

@private
consume_sequence :: proc(lexer: ^Lexer, callback: proc(r: rune) -> bool) {
    for callback(peek_current(lexer)) {
        advance_current(lexer)
    }
    return
}

@private
create_token :: proc(lexer: ^Lexer, type: Token_Type) -> (token: Token) {
    end := len(lexer.data) - len(lexer.current)
    token.lexeme = lexer.data[lexer.index:end]
    token.type   = type
    lexer.index  = end // Update for next token's lexeme to be valid
    return
}

/* 
Note
--------
 *  Is the current index the very last valid index?
 *  That is, if we were to create a token right now, attempting to use the index
    along with the calculated end length would result in an empty string?
--------
 */
@private
is_at_end :: proc(lexer: ^Lexer) -> bool {
    return lexer.index >= len(lexer.data) - 1
}

@private
peek_current :: proc(lexer: ^Lexer) -> rune {
    return cast(rune)lexer.current[0]
}

@private
advance_current :: proc(lexer: ^Lexer) {
    if is_at_end(lexer) {
        return
    }
    lexer.current = lexer.current[1:]
}

