#ifndef SCANNER_LEXER_HPP_
#define SCANNER_LEXER_HPP_

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
        template<class Iter>
        struct iterator{
            iterator() = default;

            iterator(const iterator &other) :
                place(other.place),
                char_count(other.char_count), line_count(other.line_count),
                value(other.value)
            {}

            iterator(const Iter &iter) :
                place(iter),
                char_count(0), line_count(0),
                value(0)
            {}

            ~iterator() = default;

            iterator &operator =(const Iter &other){
                place = other;
                return *this;
            }

            iterator &operator =(const iterator &other){
                place = other.place;
                char_count = other.char_count, line_count = other.line_count;
                value = other.value;
                return *this;
            }

            iterator &operator ++(){
                if(*place == '\n'){
                    char_count = 0;
                    ++line_count;
                } else{
                    ++char_count;
                }
                ++place;
                return *this;
            }

            typename Iter::value_type &operator *() const{
                return *place;
            }

            bool operator ==(const Iter &other) const{
                Iter last = place;
                --last;
                return last == other;
            }

            bool operator ==(const iterator &other) const{
                return place == other.place;
            }

            template<class Other>
            bool operator !=(const Other &other) const{
                return !(*this == other);
            }

            Iter place;
            std::size_t char_count, line_count;
            scanner_type::term_type value;
        };

        namespace impl{
            class lexer{
            public:
                template<class InputIter>
                static std::pair<bool, InputIter> reg_whitespace(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; } else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter == last){ match = false; } else{
                                InputIter iter_prime = iter;
                                do{
                                    if(iter != last && *iter == ' '){
                                        ++iter;
                                        match = true;
                                    } else{ match = false; }
                                    if(!match){ iter = iter_prime; break; }
                                } while(false);
                            }
                            if(match){ break; } else{ iter = iter_prime; }
                            if(iter != last && *iter == '\t'){
                                ++iter;
                                match = true;
                            } else{ match = false; }
                            if(!match){ iter = iter_prime; }
                        } while(false);
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_end_of_line(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; } else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == '\n'){
                                ++iter;
                                match = true;
                            } else{ match = false; }
                            if(match){ break; } else{ iter = iter_prime; }
                            if(iter == last){ match = false; } else{
                                InputIter iter_prime = iter;
                                do{
                                    if(iter == last){ match = false; } else{
                                        InputIter iter_prime = iter;
                                        do{
                                            if(iter != last && *iter == '/'){
                                                ++iter;
                                                match = true;
                                            } else{ match = false; }
                                            if(!match){ iter = iter_prime; break; }
                                            if(iter != last && *iter == '/'){
                                                ++iter;
                                                match = true;
                                            } else{ match = false; }
                                            if(!match){ iter = iter_prime; break; }
                                        } while(false);
                                    }
                                    if(!match){ iter = iter_prime; break; }
                                    {
                                        InputIter iter_prime = iter;
                                        while(iter != last){
                                            if(iter == last){ match = false; } else{
                                                char c = *iter;
                                                if(
                                                    (c != '\n')
                                                    ){
                                                    ++iter;
                                                    match = true;
                                                } else{ match = false; }
                                            }
                                            if(match){ iter_prime = iter; } else{
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
                                    } else{ match = false; }
                                    if(!match){ iter = iter_prime; break; }
                                } while(false);
                            }
                            if(!match){ iter = iter_prime; }
                        } while(false);
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_first_line(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; } else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter == last){ match = false; } else{
                                InputIter iter_prime = iter;
                                do{
                                    if(iter != last && *iter == '#'){
                                        ++iter;
                                        match = true;
                                    } else{ match = false; }
                                    if(!match){ iter = iter_prime; break; }
                                    if(iter != last && *iter == '!'){
                                        ++iter;
                                        match = true;
                                    } else{ match = false; }
                                    if(!match){ iter = iter_prime; break; }
                                } while(false);
                            }
                            if(!match){ iter = iter_prime; break; }
                            {
                                InputIter iter_prime = iter;
                                while(iter != last){
                                    if(iter == last){ match = false; } else{
                                        char c = *iter;
                                        if(
                                            (c != '\n')
                                            ){
                                            ++iter;
                                            match = true;
                                        } else{ match = false; }
                                    }
                                    if(match){ iter_prime = iter; } else{
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
                            } else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        } while(false);
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_identifier(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; } else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter == last){ match = false; } else{
                                char c = *iter;
                                if(
                                    ((c >= 'a') && (c <= 'z')) ||
                                    ((c >= 'A') && (c <= 'Z')) ||
                                    (c == '_')
                                    ){
                                    ++iter;
                                    match = true;
                                } else{ match = false; }
                            }
                            if(!match){ iter = iter_prime; break; }
                            {
                                InputIter iter_prime = iter;
                                while(iter != last){
                                    if(iter == last){ match = false; } else{
                                        char c = *iter;
                                        if(
                                            ((c >= 'a') && (c <= 'z')) ||
                                            ((c >= 'A') && (c <= 'Z')) ||
                                            ((c >= '0') && (c <= '9')) ||
                                            (c == '_')
                                            ){
                                            ++iter;
                                            match = true;
                                        } else{ match = false; }
                                    }
                                    if(match){ iter_prime = iter; } else{
                                        iter = iter_prime;
                                        match = true;
                                        break;
                                    }
                                }
                            }
                            if(!match){ iter = iter_prime; break; }
                        } while(false);
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_value(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; } else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter == last){ match = false; } else{
                                InputIter iter_prime = iter;
                                do{
                                    if(iter == last){ match = false; } else{
                                        char c = *iter;
                                        if(
                                            ((c >= '1') && (c <= '9'))
                                            ){
                                            ++iter;
                                            match = true;
                                        } else{ match = false; }
                                    }
                                    if(!match){ iter = iter_prime; break; }
                                    {
                                        InputIter iter_prime = iter;
                                        while(iter != last){
                                            if(iter == last){ match = false; } else{
                                                char c = *iter;
                                                if(
                                                    ((c >= '0') && (c <= '9'))
                                                    ){
                                                    ++iter;
                                                    match = true;
                                                } else{ match = false; }
                                            }
                                            if(match){ iter_prime = iter; } else{
                                                iter = iter_prime;
                                                match = true;
                                                break;
                                            }
                                        }
                                    }
                                    if(!match){ iter = iter_prime; break; }
                                } while(false);
                            }
                            if(match){ break; } else{ iter = iter_prime; }
                            if(iter == last){ match = false; } else{
                                InputIter iter_prime = iter;
                                do{
                                    if(iter != last && *iter == '0'){
                                        ++iter;
                                        match = true;
                                    } else{ match = false; }
                                    if(!match){ iter = iter_prime; break; }
                                } while(false);
                            }
                            if(!match){ iter = iter_prime; }
                        } while(false);
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_comma(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; } else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == ','){
                                ++iter;
                                match = true;
                            } else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        } while(false);
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_dot(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; } else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == '.'){
                                ++iter;
                                match = true;
                            } else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        } while(false);
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_asterisk(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; } else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == '*'){
                                ++iter;
                                match = true;
                            } else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        } while(false);
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_ampersand(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; } else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == '&'){
                                ++iter;
                                match = true;
                            } else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        } while(false);
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_double_colon(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; } else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == ':'){
                                ++iter;
                                match = true;
                            } else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                            if(iter != last && *iter == ':'){
                                ++iter;
                                match = true;
                            } else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        } while(false);
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_semicolon(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; } else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == ';'){
                                ++iter;
                                match = true;
                            } else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        } while(false);
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_l_square_bracket(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; } else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == '['){
                                ++iter;
                                match = true;
                            } else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        } while(false);
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_r_square_bracket(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; } else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == ']'){
                                ++iter;
                                match = true;
                            } else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        } while(false);
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_l_curly_bracket(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; } else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == '{'){
                                ++iter;
                                match = true;
                            } else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        } while(false);
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_r_curly_bracket(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; } else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == '}'){
                                ++iter;
                                match = true;
                            } else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        } while(false);
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_l_bracket(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; } else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == '<'){
                                ++iter;
                                match = true;
                            } else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        } while(false);
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_r_bracket(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; } else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == '>'){
                                ++iter;
                                match = true;
                            } else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        } while(false);
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_l_round_pare(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; } else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == '('){
                                ++iter;
                                match = true;
                            } else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        } while(false);
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_r_round_pare(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; } else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == ')'){
                                ++iter;
                                match = true;
                            } else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        } while(false);
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_symbol_or(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; } else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == '|'){
                                ++iter;
                                match = true;
                            } else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        } while(false);
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_symbol_colon(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; } else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == ':'){
                                ++iter;
                                match = true;
                            } else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        } while(false);
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter>
                static std::pair<bool, InputIter> reg_str(InputIter first, InputIter last){
                    InputIter iter = first;
                    bool match = true;
                    if(iter == last){ match = false; } else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == '"'){
                                ++iter;
                                match = true;
                            } else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                            do{
                                if(iter == last){ match = false; } else{
                                    ++iter;
                                    match = true;
                                }
                                if(!match){ break; }
                                InputIter iter_prime = iter;
                                while(iter != last){
                                    if(iter == last){ match = false; } else{
                                        ++iter;
                                        match = true;
                                    }
                                    if(match){ iter_prime = iter; } else{
                                        iter = iter_prime;
                                        match = true;
                                        break;
                                    }
                                }
                            } while(false);
                            if(!match){ iter = iter_prime; break; }
                            if(iter != last && *iter == '"'){
                                ++iter;
                                match = true;
                            } else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        } while(false);
                    }
                    return std::make_pair(match, iter);
                }

                template<class InputIter, class InsertFunctor>
                inline static std::pair<bool, iterator<InputIter>> tokenize(
                    iterator<InputIter> first,
                    iterator<InputIter> last,
                    const InsertFunctor &f
                ){
                    iterator<InputIter> iter = first;
                    std::pair<bool, iterator<InputIter>> result;
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
                            f(terminal_symbol::identifier, iter, result.second);
                            iter = result.second;
                            continue;
                        }
                        result = reg_value(iter, last);
                        if(result.first){
                            f(terminal_symbol::value, iter, result.second);
                            iter = result.second;
                            continue;
                        }
                        result = reg_comma(iter, last);
                        if(result.first){
                            f(terminal_symbol::comma, iter, result.second);
                            iter = result.second;
                            continue;
                        }
                        result = reg_dot(iter, last);
                        if(result.first){
                            f(terminal_symbol::dot, iter, result.second);
                            iter = result.second;
                            continue;
                        }
                        result = reg_asterisk(iter, last);
                        if(result.first){
                            f(terminal_symbol::asterisk, iter, result.second);
                            iter = result.second;
                            continue;
                        }
                        result = reg_ampersand(iter, last);
                        if(result.first){
                            f(terminal_symbol::ampersand, iter, result.second);
                            iter = result.second;
                            continue;
                        }
                        result = reg_double_colon(iter, last);
                        if(result.first){
                            f(terminal_symbol::double_colon, iter, result.second);
                            iter = result.second;
                            continue;
                        }
                        result = reg_semicolon(iter, last);
                        if(result.first){
                            f(terminal_symbol::semicolon, iter, result.second);
                            iter = result.second;
                            continue;
                        }
                        result = reg_l_square_bracket(iter, last);
                        if(result.first){
                            f(terminal_symbol::l_square_bracket, iter, result.second);
                            iter = result.second;
                            continue;
                        }
                        result = reg_r_square_bracket(iter, last);
                        if(result.first){
                            f(terminal_symbol::r_square_bracket, iter, result.second);
                            iter = result.second;
                            continue;
                        }
                        result = reg_l_curly_bracket(iter, last);
                        if(result.first){
                            f(terminal_symbol::l_curly_bracket, iter, result.second);
                            iter = result.second;
                            continue;
                        }
                        result = reg_r_curly_bracket(iter, last);
                        if(result.first){
                            f(terminal_symbol::r_curly_bracket, iter, result.second);
                            iter = result.second;
                            continue;
                        }
                        result = reg_l_bracket(iter, last);
                        if(result.first){
                            f(terminal_symbol::l_bracket, iter, result.second);
                            iter = result.second;
                            continue;
                        }
                        result = reg_r_bracket(iter, last);
                        if(result.first){
                            f(terminal_symbol::r_bracket, iter, result.second);
                            iter = result.second;
                            continue;
                        }
                        result = reg_l_round_pare(iter, last);
                        if(result.first){
                            f(terminal_symbol::l_round_pare, iter, result.second);
                            iter = result.second;
                            continue;
                        }
                        result = reg_r_round_pare(iter, last);
                        if(result.first){
                            f(terminal_symbol::r_round_pare, iter, result.second);
                            iter = result.second;
                            continue;
                        }
                        result = reg_symbol_or(iter, last);
                        if(result.first){
                            f(terminal_symbol::symbol_or, iter, result.second);
                            iter = result.second;
                            continue;
                        }
                        result = reg_symbol_colon(iter, last);
                        if(result.first){
                            f(terminal_symbol::symbol_colon, iter, result.second);
                            iter = result.second;
                            continue;
                        }
                        result = reg_str(iter, last);
                        if(result.first){
                            f(terminal_symbol::str, iter, result.second);
                            iter = result.second;
                            continue;
                        }
                        break;
                    }
                    return result;
                }
            };
        }
    }// namespace scanner
}

#endif // SCANNER_LEXER_HPP_

