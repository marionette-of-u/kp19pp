#ifndef KP19PP_SCANNER_LEXER_HPP_
#define KP19PP_SCANNER_LEXER_HPP_

namespace kp19pp{
    namespace scanner{
        enum token{
            token_0,
            token_whitespace,
            token_end_of_line,
            token_first_line,
            token_identifier,
            token_value,
            token_comma,
            token_dot,
            token_asterisk,
            token_ampersand,
            token_double_colon,
            token_semicolon,
            token_l_square_bracket,
            token_r_square_bracket,
            token_l_curly_bracket,
            token_r_curly_bracket,
            token_l_bracket,
            token_r_bracket,
            token_l_round_pare,
            token_r_round_pare,
            token_symbol_or,
            token_symbol_colon,
            token_equal,
            token_str
        };
    }
}

#endif // KP19PP_SCANNER_LEXER_HPP_
