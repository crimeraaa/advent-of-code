package aoc

import "core:fmt"

Token :: struct {
    lexeme: string   `fmt:"q"`,
    type: Token_Type `fmt:"s"`,
}

Token_Type :: enum {
    Invalid = 0,    // Anything that does not fit anything below.
    Mul,            // "mul"
    Do,             // "do"
    Dont,           // "don't"
    Number,         // A sequence of the ASCII characters '0' to '9'.
    Left_Paren,     // The ASCII character '('.
    Right_Paren,    // The ASCII character ')'.
    Comma,          // The ASCII character ','.
    Eof,            // Not a character per se, acts more like a signal.
}

token_print :: proc(token: Token) {
    fmt.printfln("%s: %q", token.type, token.lexeme)
}
