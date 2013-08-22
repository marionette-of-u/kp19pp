#include <utility>
#include <iterator>
#include <cstring>
#include <cctype>
#include <ctype.h>
#include <boost/range.hpp>
#include "scanner.hpp"
#include "exception.hpp"

namespace kp19pp{
    namespace scanner{
        lexer *lex;

        lexer::lexer(scanner_type::token_seq_type &token_seq_){
            reset(&token_seq_);
            lex = this;
        }

        void lexer::reset(scanner_type::token_seq_type *token_seq_){
            char_count() = 0;
            line_count() = 0;
            token_seq() = token_seq_;
        }

        namespace impl{
            class lexer{
            public:
                template<class InputIter>
                static std::pair<bool, InputIter> reg_whitespace(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; }else{ 
                        InputIter iter_prime = iter;
                        do{
                            if(iter == last){ match = false; }else{
                                InputIter iter_prime = iter;
                                do{
                                    if(iter != last && *iter == ' '){
                                        ++iter;
                                        match = true;
                                    }else{ match = false; }
                                    if(!match){ iter = iter_prime; break; }
                                }while(false);
                            }
                            if(match){ break; }else{ iter = iter_prime; }
                            if(iter != last && *iter == '\t'){
                                ++iter;
                                match = true;
                            }else{ match = false; }
                            if(!match){ iter = iter_prime; }
                        }while(false);
                    }
                    if(match){
                        scanner::lexer::char_count() += last - iter;
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_end_of_line(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; }else{ 
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == '\n'){
                                ++iter;
                                match = true;
                            }else{ match = false; }
                            if(match){ break; }else{ iter = iter_prime; }
                            if(iter == last){ match = false; }else{
                                InputIter iter_prime = iter;
                                do{
                                    if(iter == last){ match = false; }else{
                                        InputIter iter_prime = iter;
                                        do{
                                            if(iter != last && *iter == '/'){
                                                ++iter;
                                                match = true;
                                            }else{ match = false; }
                                            if(!match){ iter = iter_prime; break; }
                                            if(iter != last && *iter == '/'){
                                                ++iter;
                                                match = true;
                                            }else{ match = false; }
                                            if(!match){ iter = iter_prime; break; }
                                        }while(false);
                                    }
                                    if(!match){ iter = iter_prime; break; }
                                    {
                                        InputIter iter_prime = iter;
                                        while(iter != last){
                                            if(iter == last){ match = false; }else{
                                                char c = *iter;
                                                if(
                                                    (c != '\n')
                                                ){
                                                    ++iter;
                                                    match = true;
                                                }else{ match = false; }
                                            }
                                            if(match){ iter_prime = iter; }else{
                                                iter = iter_prime;
                                                match = true;
                                                break;
                                            }
                                        }
                                    }
                                    if(!match){ iter = iter_prime; break; }
                                    if(iter != last && *iter == '\n'){
                                        ++iter;
                                        match = true;
                                    }else{ match = false; }
                                    if(!match){ iter = iter_prime; break; }
                                }while(false);
                            }
                            if(!match){ iter = iter_prime; }
                        }while(false);
                    }
                    if(match){
                        scanner::lexer::char_count() = 0;
                        ++scanner::lexer::line_count();
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_first_line(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; }else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter == last){ match = false; }else{
                                InputIter iter_prime = iter;
                                do{
                                    if(iter != last && *iter == '#'){
                                        ++iter;
                                        match = true;
                                    }else{ match = false; }
                                    if(!match){ iter = iter_prime; break; }
                                    if(iter != last && *iter == '!'){
                                        ++iter;
                                        match = true;
                                    }else{ match = false; }
                                    if(!match){ iter = iter_prime; break; }
                                }while(false);
                            }
                            if(!match){ iter = iter_prime; break; }
                            {
                                InputIter iter_prime = iter;
                                while(iter != last){
                                    if(iter == last){ match = false; }else{
                                        char c = *iter;
                                        if(
                                            (c != '\n')
                                        ){
                                            ++iter;
                                            match = true;
                                        }else{ match = false; }
                                    }
                                    if(match){ iter_prime = iter; }else{
                                        iter = iter_prime;
                                        match = true;
                                        break;
                                    }
                                }
                            }
                            if(!match){ iter = iter_prime; break; }
                            if(iter != last && *iter == '\n'){
                                ++iter;
                                match = true;
                            }else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        }while(false);
                    }
                    if(match){
                        if(scanner::lexer::line_count() == 0){
                            scanner::lexer::char_count() = 0;
                            ++scanner::lexer::line_count();
                        }else{
                            throw(exception("lexical error.", lex->char_count(), lex->line_count()));
                        }
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_identifier(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; }else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter == last){ match = false; }else{
                                char c = *iter;
                                if(
                                    ((c >= 'a') && (c <= 'z')) ||
                                    ((c >= 'A') && (c <= 'Z')) ||
                                    (c == '_')
                                ){
                                    ++iter;
                                    match = true;
                                }else{ match = false; }
                            }
                            if(!match){ iter = iter_prime; break; }
                            {
                                InputIter iter_prime = iter;
                                while(iter != last){
                                    if(iter == last){ match = false; }else{
                                        char c = *iter;
                                        if(
                                            ((c >= 'a') && (c <= 'z')) ||
                                            ((c >= 'A') && (c <= 'Z')) ||
                                            ((c >= '0') && (c <= '9')) ||
                                            (c == '_')
                                        ){
                                            ++iter;
                                            match = true;
                                        }else{ match = false; }
                                    }
                                    if(match){ iter_prime = iter; }else{
                                        iter = iter_prime;
                                        match = true;
                                        break;
                                    }
                                }
                            }
                            if(!match){ iter = iter_prime; break; }
                        }while(false);
                    }
                    if(match){
                        scanner::lexer::token_seq()->push_back(
                            scanner_type::token_type(
                                string_iter_pair_type(first, iter),
                                terminal_symbol::identifier,
                                lex->char_count(),
                                lex->line_count()
                            )
                        );
                        lex->char_count() += iter - first;
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_value(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; }else{ 
                        InputIter iter_prime = iter;
                        do{
                            if(iter == last){ match = false; }else{
                                InputIter iter_prime = iter;
                                do{
                                    if(iter == last){ match = false; }else{
                                        char c = *iter;
                                        if(
                                            ((c >= '1') && (c <= '9'))
                                        ){
                                            ++iter;
                                            match = true;
                                        }else{ match = false; }
                                    }
                                    if(!match){ iter = iter_prime; break; }
                                    {
                                        InputIter iter_prime = iter;
                                        while(iter != last){
                                            if(iter == last){ match = false; }else{
                                                char c = *iter;
                                                if(
                                                    ((c >= '0') && (c <= '9'))
                                                ){
                                                    ++iter;
                                                    match = true;
                                                }else{ match = false; }
                                            }
                                            if(match){ iter_prime = iter; }else{
                                                iter = iter_prime;
                                                match = true;
                                                break;
                                            }
                                        }
                                    }
                                    if(!match){ iter = iter_prime; break; }
                                }while(false);
                            }
                            if(match){ break; }else{ iter = iter_prime; }
                            if(iter == last){ match = false; }else{
                                InputIter iter_prime = iter;
                                do{
                                    if(iter != last && *iter == '0'){
                                        ++iter;
                                        match = true;
                                    }else{ match = false; }
                                    if(!match){ iter = iter_prime; break; }
                                }while(false);
                            }
                            if(!match){ iter = iter_prime; }
                        }while(false);
                    }
                    if(match){
                        scanner::lexer::token_seq()->push_back(
                            scanner_type::token_type(
                                string_iter_pair_type(first, iter),
                                terminal_symbol::value,
                                lex->char_count(),
                                lex->line_count()
                            )
                        );
                        lex->char_count() += iter - first;
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_comma(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; }else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == ','){
                                ++iter;
                                match = true;
                            }else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        }while(false);
                    }
                    if(match){
                        scanner::lexer::token_seq()->push_back(
                            scanner_type::token_type(
                                string_iter_pair_type(first, iter),
                                terminal_symbol::comma,
                                lex->char_count(),
                                lex->line_count()
                            )
                        );
                        lex->char_count() += iter - first;
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_dot(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; }else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == '.'){
                                ++iter;
                                match = true;
                            }else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        }while(false);
                    }
                    if(match){
                        scanner::lexer::token_seq()->push_back(
                            scanner_type::token_type(
                                string_iter_pair_type(first, iter),
                                terminal_symbol::dot,
                                lex->char_count(),
                                lex->line_count()
                            )
                        );
                        lex->char_count() += iter - first;
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_asterisk(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; }else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == '*'){
                                ++iter;
                                match = true;
                            }else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        }while(false);
                    }
                    if(match){
                        scanner::lexer::token_seq()->push_back(
                            scanner_type::token_type(
                                string_iter_pair_type(first, iter),
                                terminal_symbol::asterisk,
                                lex->char_count(),
                                lex->line_count()
                            )
                        );
                        lex->char_count() += iter - first;
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_ampersand(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; }else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == '&'){
                                ++iter;
                                match = true;
                            }else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        }while(false);
                    }
                    if(match){
                        scanner::lexer::token_seq()->push_back(
                            scanner_type::token_type(
                                string_iter_pair_type(first, iter),
                                terminal_symbol::ampersand,
                                lex->char_count(),
                                lex->line_count()
                            )
                        );
                        lex->char_count() += iter - first;
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_double_colon(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; }else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == ':'){
                                ++iter;
                                match = true;
                            }else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                            if(iter != last && *iter == ':'){
                                ++iter;
                                match = true;
                            }else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        }while(false);
                    }
                    if(match){
                        scanner::lexer::token_seq()->push_back(
                            scanner_type::token_type(
                                string_iter_pair_type(first, iter),
                                terminal_symbol::double_colon,
                                lex->char_count(),
                                lex->line_count()
                            )
                        );
                        lex->char_count() += iter - first;
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_semicolon(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; }else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == ';'){
                                ++iter;
                                match = true;
                            }else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        }while(false);
                    }
                    if(match){
                        scanner::lexer::token_seq()->push_back(
                            scanner_type::token_type(
                                string_iter_pair_type(first, iter),
                                terminal_symbol::semicolon,
                                lex->char_count(),
                                lex->line_count()
                            )
                        );
                        lex->char_count() += iter - first;
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_l_square_bracket(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; }else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == '['){
                                ++iter;
                                match = true;
                            }else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        }while(false);
                    }
                    if(match){
                        scanner::lexer::token_seq()->push_back(
                            scanner_type::token_type(
                                string_iter_pair_type(first, iter),
                                terminal_symbol::l_square_bracket,
                                lex->char_count(),
                                lex->line_count()
                            )
                        );
                        lex->char_count() += iter - first;
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_r_square_bracket(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; }else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == ']'){
                                ++iter;
                                match = true;
                            }else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        }while(false);
                    }
                    if(match){
                        scanner::lexer::token_seq()->push_back(
                            scanner_type::token_type(
                                string_iter_pair_type(first, iter),
                                terminal_symbol::r_square_bracket,
                                lex->char_count(),
                                lex->line_count()
                            )
                        );
                        lex->char_count() += iter - first;
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_l_curly_bracket(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; }else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == '{'){
                                ++iter;
                                match = true;
                            }else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        }while(false);
                    }
                    if(match){
                        scanner::lexer::token_seq()->push_back(
                            scanner_type::token_type(
                                string_iter_pair_type(first, iter),
                                terminal_symbol::l_curly_bracket,
                                lex->char_count(),
                                lex->line_count()
                            )
                        );
                        lex->char_count() += iter - first;
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_r_curly_bracket(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; }else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == '}'){
                                ++iter;
                                match = true;
                            }else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        }while(false);
                    }
                    if(match){
                        scanner::lexer::token_seq()->push_back(
                            scanner_type::token_type(
                                string_iter_pair_type(first, iter),
                                terminal_symbol::r_curly_bracket,
                                lex->char_count(),
                                lex->line_count()
                            )
                        );
                        lex->char_count() += iter - first;
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_l_bracket(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; }else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == '<'){
                                ++iter;
                                match = true;
                            }else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        }while(false);
                    }
                    if(match){
                        scanner::lexer::token_seq()->push_back(
                            scanner_type::token_type(
                                string_iter_pair_type(first, iter),
                                terminal_symbol::l_bracket,
                                lex->char_count(),
                                lex->line_count()
                            )
                        );
                        lex->char_count() += iter - first;
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_r_bracket(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; }else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == '>'){
                                ++iter;
                                match = true;
                            }else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        }while(false);
                    }
                    if(match){
                        scanner::lexer::token_seq()->push_back(
                            scanner_type::token_type(
                                string_iter_pair_type(first, iter),
                                terminal_symbol::r_bracket,
                                lex->char_count(),
                                lex->line_count()
                            )
                        );
                        lex->char_count() += iter - first;
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_l_round_pare(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; }else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == '('){
                                ++iter;
                                match = true;
                            }else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        }while(false);
                    }
                    if(match){
                        scanner::lexer::token_seq()->push_back(
                            scanner_type::token_type(
                                string_iter_pair_type(first, iter),
                                terminal_symbol::l_round_pare,
                                lex->char_count(),
                                lex->line_count()
                            )
                        );
                        lex->char_count() += iter - first;
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_r_round_pare(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; }else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == ')'){
                                ++iter;
                                match = true;
                            }else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        }while(false);
                    }
                    if(match){
                        scanner::lexer::token_seq()->push_back(
                            scanner_type::token_type(
                                string_iter_pair_type(first, iter),
                                terminal_symbol::r_round_pare,
                                lex->char_count(),
                                lex->line_count()
                            )
                        );
                        lex->char_count() += iter - first;
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_symbol_or(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; }else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == '|'){
                                ++iter;
                                match = true;
                            }else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        }while(false);
                    }
                    if(match){
                        scanner::lexer::token_seq()->push_back(
                            scanner_type::token_type(
                                string_iter_pair_type(first, iter),
                                terminal_symbol::symbol_or,
                                lex->char_count(),
                                lex->line_count()
                            )
                        );
                        lex->char_count() += iter - first;
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_symbol_colon(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; }else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == ':'){
                                ++iter;
                                match = true;
                            }else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        }while(false);
                    }
                    if(match){
                        scanner::lexer::token_seq()->push_back(
                            scanner_type::token_type(
                                string_iter_pair_type(first, iter),
                                terminal_symbol::symbol_colon,
                                lex->char_count(),
                                lex->line_count()
                            )
                        );
                        lex->char_count() += iter - first;
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static void tokenize(InputIter first, InputIter last){
                    InputIter iter = first;
                    std::pair<bool, InputIter> result;
                    while(iter != last){
                        result = reg_whitespace(iter, last);
                        if(result.first){
                            iter = result.second;
                            continue;
                        }
                        result = reg_end_of_line(iter, last);
                        if(result.first){
                            iter = result.second;
                            continue;
                        }
                        result = reg_first_line(iter, last);
                        if(result.first){
                            iter = result.second;
                            continue;
                        }
                        result = reg_identifier(iter, last);
                        if(result.first){
                            iter = result.second;
                            continue;
                        }
                        result = reg_value(iter, last);
                        if(result.first){
                            iter = result.second;
                            continue;
                        }
                        result = reg_comma(iter, last);
                        if(result.first){
                            iter = result.second;
                            continue;
                        }
                        result = reg_dot(iter, last);
                        if(result.first){
                            iter = result.second;
                            continue;
                        }
                        result = reg_asterisk(iter, last);
                        if(result.first){
                            iter = result.second;
                            continue;
                        }
                        result = reg_ampersand(iter, last);
                        if(result.first){
                            iter = result.second;
                            continue;
                        }
                        result = reg_double_colon(iter, last);
                        if(result.first){
                            iter = result.second;
                            continue;
                        }
                        result = reg_semicolon(iter, last);
                        if(result.first){
                            iter = result.second;
                            continue;
                        }
                        result = reg_l_square_bracket(iter, last);
                        if(result.first){
                            iter = result.second;
                            continue;
                        }
                        result = reg_r_square_bracket(iter, last);
                        if(result.first){
                            iter = result.second;
                            continue;
                        }
                        result = reg_l_curly_bracket(iter, last);
                        if(result.first){
                            iter = result.second;
                            continue;
                        }
                        result = reg_r_curly_bracket(iter, last);
                        if(result.first){
                            iter = result.second;
                            continue;
                        }
                        result = reg_l_bracket(iter, last);
                        if(result.first){
                            iter = result.second;
                            continue;
                        }
                        result = reg_r_bracket(iter, last);
                        if(result.first){
                            iter = result.second;
                            continue;
                        }
                        result = reg_l_round_pare(iter, last);
                        if(result.first){
                            iter = result.second;
                            continue;
                        }
                        result = reg_r_round_pare(iter, last);
                        if(result.first){
                            iter = result.second;
                            continue;
                        }
                        result = reg_symbol_or(iter, last);
                        if(result.first){
                            iter = result.second;
                            continue;
                        }
                        result = reg_symbol_colon(iter, last);
                        if(result.first){
                            iter = result.second;
                            continue;
                        }
                        break;
                    }
                    if(iter != last){
                        throw(exception("lexical error.", lex->char_count(), lex->line_count()));
                    }
                }
            };
        } // namespace impl

        void lexer::tokenize(std::istream &in, scanner_string_type &str){
            {
                std::string str_;
                std::getline(in, str_, '\0');
                str.reserve(str_.size());
                str.assign(str_.begin(), str_.end());
            }
            impl::lexer::tokenize(str.begin(), str.end());
        }

        std::size_t &lexer::char_count(){
            static std::size_t value;
            return value;
        }
            
        std::size_t &lexer::line_count(){
            static std::size_t value;
            return value;
        }
            
        scanner_type::token_seq_type *&lexer::token_seq(){
            static scanner_type::token_seq_type *ptr;
            return ptr;
        }
    } // namespace scanner
} // namespace kp19pp
