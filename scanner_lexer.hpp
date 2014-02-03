#ifndef SCANNER_LEXER_HPP_
#define SCANNER_LEXER_HPP_

#include <utility>
#include <iterator>
#include <cstring>
#include <cctype>
#include <ctype.h>
#include "scanner.hpp"

namespace kp19pp{ namespace scanner{
template<class Iter>
struct iterator{
    iterator() = default;
    iterator(const iterator &other) :
        place(other.place),
        char_count(other.char_count), line_count(other.line_count)
    {}

    iterator(const Iter &iter) :
        place(iter),
        char_count(0), line_count(0)
    {}

    ~iterator() = default;
    //iterator &operator =(const Iter &other){
    //    place = other;
    //    return *this;
    //}

    iterator &operator =(const iterator &other){
        place = other.place;
        char_count = other.char_count, line_count = other.line_count;
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

    //bool operator ==(const Iter &other) const{
    //    Iter last = place;
    //    --last;
    //    return last == other;
    //}

    bool operator ==(const iterator &other) const{
        return place == other.place;
    }

    template<class Other>
    bool operator !=(const Other &other) const{
        return !(*this == other);
    }

    Iter place;
    std::size_t char_count, line_count;
};

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
                    const char *str = " ";
                    std::size_t n = sizeof(" ") / sizeof(char);
                    std::size_t i = 0;
                    while(str[i]){
                        if(str[i] != *iter){ break; }
                        ++i, ++iter;
                    }
                    match = str[i] == '\0';
                }
                if(match){ break; }else{ iter = iter_prime; }
                if(iter != last && *iter == '\t'){
                    ++iter;
                    match = true;
                }else{ match = false; }
                if(!match){ iter = iter_prime; }
            }while(false);
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
                            const char *str = "//";
                            std::size_t n = sizeof("//") / sizeof(char);
                            std::size_t i = 0;
                            while(str[i]){
                                if(str[i] != *iter){ break; }
                                ++i, ++iter;
                            }
                            match = str[i] == '\0';
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
                    const char *str = "#!";
                    std::size_t n = sizeof("#!") / sizeof(char);
                    std::size_t i = 0;
                    while(str[i]){
                        if(str[i] != *iter){ break; }
                        ++i, ++iter;
                    }
                    match = str[i] == '\0';
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
                    const char *str = "0";
                    std::size_t n = sizeof("0") / sizeof(char);
                    std::size_t i = 0;
                    while(str[i]){
                        if(str[i] != *iter){ break; }
                        ++i, ++iter;
                    }
                    match = str[i] == '\0';
                }
                if(!match){ iter = iter_prime; }
            }while(false);
        }
        return std::make_pair(match, iter);
    }

    template<class InputIter>
    static std::pair<bool, InputIter> reg_comma(InputIter first, InputIter last){
        InputIter iter = first;
        bool match = true;
        if(iter == last){ match = false; }else{
            InputIter iter_prime = iter;
            const char *str = ",";
            std::size_t n = sizeof(",") / sizeof(char);
            std::size_t i = 0;
            while(str[i]){
                if(str[i] != *iter){ break; }
                ++i, ++iter;
            }
            match = str[i] == '\0';
        }
        return std::make_pair(match, iter);
    }

    template<class InputIter>
    static std::pair<bool, InputIter> reg_dot(InputIter first, InputIter last){
        InputIter iter = first;
        bool match = true;
        if(iter == last){ match = false; }else{
            InputIter iter_prime = iter;
            const char *str = ".";
            std::size_t n = sizeof(".") / sizeof(char);
            std::size_t i = 0;
            while(str[i]){
                if(str[i] != *iter){ break; }
                ++i, ++iter;
            }
            match = str[i] == '\0';
        }
        return std::make_pair(match, iter);
    }

    template<class InputIter>
    static std::pair<bool, InputIter> reg_question(InputIter first, InputIter last){
        InputIter iter = first;
        bool match = true;
        if(iter == last){ match = false; }else{
            InputIter iter_prime = iter;
            const char *str = "?";
            std::size_t n = sizeof("?") / sizeof(char);
            std::size_t i = 0;
            while(str[i]){
                if(str[i] != *iter){ break; }
                ++i, ++iter;
            }
            match = str[i] == '\0';
        }
        return std::make_pair(match, iter);
    }

    template<class InputIter>
    static std::pair<bool, InputIter> reg_plus(InputIter first, InputIter last){
        InputIter iter = first;
        bool match = true;
        if(iter == last){ match = false; }else{
            InputIter iter_prime = iter;
            const char *str = "+";
            std::size_t n = sizeof("+") / sizeof(char);
            std::size_t i = 0;
            while(str[i]){
                if(str[i] != *iter){ break; }
                ++i, ++iter;
            }
            match = str[i] == '\0';
        }
        return std::make_pair(match, iter);
    }

    template<class InputIter>
    static std::pair<bool, InputIter> reg_asterisk(InputIter first, InputIter last){
        InputIter iter = first;
        bool match = true;
        if(iter == last){ match = false; }else{
            InputIter iter_prime = iter;
            const char *str = "*";
            std::size_t n = sizeof("*") / sizeof(char);
            std::size_t i = 0;
            while(str[i]){
                if(str[i] != *iter){ break; }
                ++i, ++iter;
            }
            match = str[i] == '\0';
        }
        return std::make_pair(match, iter);
    }

    template<class InputIter>
    static std::pair<bool, InputIter> reg_ampersand(InputIter first, InputIter last){
        InputIter iter = first;
        bool match = true;
        if(iter == last){ match = false; }else{
            InputIter iter_prime = iter;
            const char *str = "&";
            std::size_t n = sizeof("&") / sizeof(char);
            std::size_t i = 0;
            while(str[i]){
                if(str[i] != *iter){ break; }
                ++i, ++iter;
            }
            match = str[i] == '\0';
        }
        return std::make_pair(match, iter);
    }

    template<class InputIter>
    static std::pair<bool, InputIter> reg_double_colon(InputIter first, InputIter last){
        InputIter iter = first;
        bool match = true;
        if(iter == last){ match = false; }else{
            InputIter iter_prime = iter;
            const char *str = "::";
            std::size_t n = sizeof("::") / sizeof(char);
            std::size_t i = 0;
            while(str[i]){
                if(str[i] != *iter){ break; }
                ++i, ++iter;
            }
            match = str[i] == '\0';
        }
        return std::make_pair(match, iter);
    }

    template<class InputIter>
    static std::pair<bool, InputIter> reg_semicolon(InputIter first, InputIter last){
        InputIter iter = first;
        bool match = true;
        if(iter == last){ match = false; }else{
            InputIter iter_prime = iter;
            const char *str = ";";
            std::size_t n = sizeof(";") / sizeof(char);
            std::size_t i = 0;
            while(str[i]){
                if(str[i] != *iter){ break; }
                ++i, ++iter;
            }
            match = str[i] == '\0';
        }
        return std::make_pair(match, iter);
    }

    template<class InputIter>
    static std::pair<bool, InputIter> reg_l_square_bracket(InputIter first, InputIter last){
        InputIter iter = first;
        bool match = true;
        if(iter == last){ match = false; }else{
            InputIter iter_prime = iter;
            const char *str = "[";
            std::size_t n = sizeof("[") / sizeof(char);
            std::size_t i = 0;
            while(str[i]){
                if(str[i] != *iter){ break; }
                ++i, ++iter;
            }
            match = str[i] == '\0';
        }
        return std::make_pair(match, iter);
    }

    template<class InputIter>
    static std::pair<bool, InputIter> reg_r_square_bracket(InputIter first, InputIter last){
        InputIter iter = first;
        bool match = true;
        if(iter == last){ match = false; }else{
            InputIter iter_prime = iter;
            const char *str = "]";
            std::size_t n = sizeof("]") / sizeof(char);
            std::size_t i = 0;
            while(str[i]){
                if(str[i] != *iter){ break; }
                ++i, ++iter;
            }
            match = str[i] == '\0';
        }
        return std::make_pair(match, iter);
    }

    template<class InputIter>
    static std::pair<bool, InputIter> reg_l_curly_bracket(InputIter first, InputIter last){
        InputIter iter = first;
        bool match = true;
        if(iter == last){ match = false; }else{
            InputIter iter_prime = iter;
            const char *str = "{";
            std::size_t n = sizeof("{") / sizeof(char);
            std::size_t i = 0;
            while(str[i]){
                if(str[i] != *iter){ break; }
                ++i, ++iter;
            }
            match = str[i] == '\0';
        }
        return std::make_pair(match, iter);
    }

    template<class InputIter>
    static std::pair<bool, InputIter> reg_r_curly_bracket(InputIter first, InputIter last){
        InputIter iter = first;
        bool match = true;
        if(iter == last){ match = false; }else{
            InputIter iter_prime = iter;
            const char *str = "}";
            std::size_t n = sizeof("}") / sizeof(char);
            std::size_t i = 0;
            while(str[i]){
                if(str[i] != *iter){ break; }
                ++i, ++iter;
            }
            match = str[i] == '\0';
        }
        return std::make_pair(match, iter);
    }

    template<class InputIter>
    static std::pair<bool, InputIter> reg_l_bracket(InputIter first, InputIter last){
        InputIter iter = first;
        bool match = true;
        if(iter == last){ match = false; }else{
            InputIter iter_prime = iter;
            const char *str = "<";
            std::size_t n = sizeof("<") / sizeof(char);
            std::size_t i = 0;
            while(str[i]){
                if(str[i] != *iter){ break; }
                ++i, ++iter;
            }
            match = str[i] == '\0';
        }
        return std::make_pair(match, iter);
    }

    template<class InputIter>
    static std::pair<bool, InputIter> reg_r_bracket(InputIter first, InputIter last){
        InputIter iter = first;
        bool match = true;
        if(iter == last){ match = false; }else{
            InputIter iter_prime = iter;
            const char *str = ">";
            std::size_t n = sizeof(">") / sizeof(char);
            std::size_t i = 0;
            while(str[i]){
                if(str[i] != *iter){ break; }
                ++i, ++iter;
            }
            match = str[i] == '\0';
        }
        return std::make_pair(match, iter);
    }

    template<class InputIter>
    static std::pair<bool, InputIter> reg_l_round_pare(InputIter first, InputIter last){
        InputIter iter = first;
        bool match = true;
        if(iter == last){ match = false; }else{
            InputIter iter_prime = iter;
            const char *str = "(";
            std::size_t n = sizeof("(") / sizeof(char);
            std::size_t i = 0;
            while(str[i]){
                if(str[i] != *iter){ break; }
                ++i, ++iter;
            }
            match = str[i] == '\0';
        }
        return std::make_pair(match, iter);
    }

    template<class InputIter>
    static std::pair<bool, InputIter> reg_r_round_pare(InputIter first, InputIter last){
        InputIter iter = first;
        bool match = true;
        if(iter == last){ match = false; }else{
            InputIter iter_prime = iter;
            const char *str = ")";
            std::size_t n = sizeof(")") / sizeof(char);
            std::size_t i = 0;
            while(str[i]){
                if(str[i] != *iter){ break; }
                ++i, ++iter;
            }
            match = str[i] == '\0';
        }
        return std::make_pair(match, iter);
    }

    template<class InputIter>
    static std::pair<bool, InputIter> reg_symbol_or(InputIter first, InputIter last){
        InputIter iter = first;
        bool match = true;
        if(iter == last){ match = false; }else{
            InputIter iter_prime = iter;
            const char *str = "|";
            std::size_t n = sizeof("|") / sizeof(char);
            std::size_t i = 0;
            while(str[i]){
                if(str[i] != *iter){ break; }
                ++i, ++iter;
            }
            match = str[i] == '\0';
        }
        return std::make_pair(match, iter);
    }

    template<class InputIter>
    static std::pair<bool, InputIter> reg_symbol_colon(InputIter first, InputIter last){
        InputIter iter = first;
        bool match = true;
        if(iter == last){ match = false; }else{
            InputIter iter_prime = iter;
            const char *str = ":";
            std::size_t n = sizeof(":") / sizeof(char);
            std::size_t i = 0;
            while(str[i]){
                if(str[i] != *iter){ break; }
                ++i, ++iter;
            }
            match = str[i] == '\0';
        }
        return std::make_pair(match, iter);
    }

    template<class InputIter>
    static std::pair<bool, InputIter> reg_equal(InputIter first, InputIter last){
        InputIter iter = first;
        bool match = true;
        if(iter == last){ match = false; }else{
            InputIter iter_prime = iter;
            const char *str = "=";
            std::size_t n = sizeof("=") / sizeof(char);
            std::size_t i = 0;
            while(str[i]){
                if(str[i] != *iter){ break; }
                ++i, ++iter;
            }
            match = str[i] == '\0';
        }
        return std::make_pair(match, iter);
    }

    template<class InputIter>
    static std::pair<bool, InputIter> reg_str(InputIter first, InputIter last){
        InputIter iter = first;
        bool match = true;
        if(iter == last){ match = false; }else{
            InputIter iter_prime = iter;
            do{
                if(iter != last && *iter == '"'){
                    ++iter;
                    match = true;
                }else{ match = false; }
                if(!match){ iter = iter_prime; break; }
                do{
                    if(iter == last){ match = false; }else{
                        ++iter;
                        match = true;
                    }
                    if(!match){ break; }
                    InputIter iter_prime = iter;
                    while(iter != last){
                        if(iter == last){ match = false; }else{
                            ++iter;
                            match = true;
                        }
                        if(match){ iter_prime = iter; }else{
                            iter = iter_prime;
                            match = true;
                            break;
                        }
                    }
                }while(false);
                if(!match){ iter = iter_prime; break; }
                if(iter != last && *iter == '"'){
                    ++iter;
                    match = true;
                }else{ match = false; }
                if(!match){ iter = iter_prime; break; }
            }while(false);
        }
        return std::make_pair(match, iter);
    }

    template<class InputIter>
    static std::pair<token, iterator<InputIter>> apply(InputIter first, InputIter last){
        iterator<InputIter> iter = first;
        std::pair<bool, iterator<InputIter>> result;
        result = reg_whitespace(iter, last);
        if(result.first){
            iter = result.second;;
            return std::make_pair(token_whitespace, iter);
        }
        result = reg_end_of_line(iter, last);
        if(result.first){
            iter = result.second;;
            return std::make_pair(token_end_of_line, iter);
        }
        result = reg_first_line(iter, last);
        if(result.first){
            iter = result.second;;
            return std::make_pair(token_first_line, iter);
        }
        result = reg_identifier(iter, last);
        if(result.first){
            iter = result.second;;
            return std::make_pair(token_identifier, iter);
        }
        result = reg_value(iter, last);
        if(result.first){
            iter = result.second;;
            return std::make_pair(token_value, iter);
        }
        result = reg_comma(iter, last);
        if(result.first){
            iter = result.second;;
            return std::make_pair(token_comma, iter);
        }
        result = reg_dot(iter, last);
        if(result.first){
            iter = result.second;;
            return std::make_pair(token_dot, iter);
        }
        result = reg_question(iter, last);
        if(result.first){
            iter = result.second;;
            return std::make_pair(token_question, iter);
        }
        result = reg_plus(iter, last);
        if(result.first){
            iter = result.second;;
            return std::make_pair(token_plus, iter);
        }
        result = reg_asterisk(iter, last);
        if(result.first){
            iter = result.second;;
            return std::make_pair(token_asterisk, iter);
        }
        result = reg_ampersand(iter, last);
        if(result.first){
            iter = result.second;;
            return std::make_pair(token_ampersand, iter);
        }
        result = reg_double_colon(iter, last);
        if(result.first){
            iter = result.second;;
            return std::make_pair(token_double_colon, iter);
        }
        result = reg_semicolon(iter, last);
        if(result.first){
            iter = result.second;;
            return std::make_pair(token_semicolon, iter);
        }
        result = reg_l_square_bracket(iter, last);
        if(result.first){
            iter = result.second;;
            return std::make_pair(token_l_square_bracket, iter);
        }
        result = reg_r_square_bracket(iter, last);
        if(result.first){
            iter = result.second;;
            return std::make_pair(token_r_square_bracket, iter);
        }
        result = reg_l_curly_bracket(iter, last);
        if(result.first){
            iter = result.second;;
            return std::make_pair(token_l_curly_bracket, iter);
        }
        result = reg_r_curly_bracket(iter, last);
        if(result.first){
            iter = result.second;;
            return std::make_pair(token_r_curly_bracket, iter);
        }
        result = reg_l_bracket(iter, last);
        if(result.first){
            iter = result.second;;
            return std::make_pair(token_l_bracket, iter);
        }
        result = reg_r_bracket(iter, last);
        if(result.first){
            iter = result.second;;
            return std::make_pair(token_r_bracket, iter);
        }
        result = reg_l_round_pare(iter, last);
        if(result.first){
            iter = result.second;;
            return std::make_pair(token_l_round_pare, iter);
        }
        result = reg_r_round_pare(iter, last);
        if(result.first){
            iter = result.second;;
            return std::make_pair(token_r_round_pare, iter);
        }
        result = reg_symbol_or(iter, last);
        if(result.first){
            iter = result.second;;
            return std::make_pair(token_symbol_or, iter);
        }
        result = reg_symbol_colon(iter, last);
        if(result.first){
            iter = result.second;;
            return std::make_pair(token_symbol_colon, iter);
        }
        result = reg_equal(iter, last);
        if(result.first){
            iter = result.second;;
            return std::make_pair(token_equal, iter);
        }
        result = reg_str(iter, last);
        if(result.first){
            iter = result.second;;
            return std::make_pair(token_str, iter);
        }
        return std::make_pair(token_0, iter);
    }

    template<class InputIter, class InsertFunctor>
    static std::pair<bool, iterator<InputIter>> tokenize(InputIter first, InputIter last, const InsertFunctor &f){
        iterator<InputIter> iter = first, end = last;
        std::pair<bool, iterator<InputIter>> result;
        while(iter != end){
            result = reg_whitespace(iter, end);
            if(result.first){
                iter = result.second;;
                continue;
            }
            result = reg_end_of_line(iter, end);
            if(result.first){
                iter = result.second;;
                continue;
            }
            result = reg_first_line(iter, end);
            if(result.first){
                iter = result.second;;
                continue;
            }
            result = reg_identifier(iter, end);
            if(result.first){
                f(token_identifier, iter, result.second);
                iter = result.second;;
                continue;
            }
            result = reg_value(iter, end);
            if(result.first){
                f(token_value, iter, result.second);
                iter = result.second;;
                continue;
            }
            result = reg_comma(iter, end);
            if(result.first){
                f(token_comma, iter, result.second);
                iter = result.second;;
                continue;
            }
            result = reg_dot(iter, end);
            if(result.first){
                f(token_dot, iter, result.second);
                iter = result.second;;
                continue;
            }
            result = reg_question(iter, end);
            if(result.first){
                f(token_question, iter, result.second);
                iter = result.second;;
                continue;
            }
            result = reg_plus(iter, end);
            if(result.first){
                f(token_plus, iter, result.second);
                iter = result.second;;
                continue;
            }
            result = reg_asterisk(iter, end);
            if(result.first){
                f(token_asterisk, iter, result.second);
                iter = result.second;;
                continue;
            }
            result = reg_ampersand(iter, end);
            if(result.first){
                f(token_ampersand, iter, result.second);
                iter = result.second;;
                continue;
            }
            result = reg_double_colon(iter, end);
            if(result.first){
                f(token_double_colon, iter, result.second);
                iter = result.second;;
                continue;
            }
            result = reg_semicolon(iter, end);
            if(result.first){
                f(token_semicolon, iter, result.second);
                iter = result.second;;
                continue;
            }
            result = reg_l_square_bracket(iter, end);
            if(result.first){
                f(token_l_square_bracket, iter, result.second);
                iter = result.second;;
                continue;
            }
            result = reg_r_square_bracket(iter, end);
            if(result.first){
                f(token_r_square_bracket, iter, result.second);
                iter = result.second;;
                continue;
            }
            result = reg_l_curly_bracket(iter, end);
            if(result.first){
                f(token_l_curly_bracket, iter, result.second);
                iter = result.second;;
                continue;
            }
            result = reg_r_curly_bracket(iter, end);
            if(result.first){
                f(token_r_curly_bracket, iter, result.second);
                iter = result.second;;
                continue;
            }
            result = reg_l_bracket(iter, end);
            if(result.first){
                f(token_l_bracket, iter, result.second);
                iter = result.second;;
                continue;
            }
            result = reg_r_bracket(iter, end);
            if(result.first){
                f(token_r_bracket, iter, result.second);
                iter = result.second;;
                continue;
            }
            result = reg_l_round_pare(iter, end);
            if(result.first){
                f(token_l_round_pare, iter, result.second);
                iter = result.second;;
                continue;
            }
            result = reg_r_round_pare(iter, end);
            if(result.first){
                f(token_r_round_pare, iter, result.second);
                iter = result.second;;
                continue;
            }
            result = reg_symbol_or(iter, end);
            if(result.first){
                f(token_symbol_or, iter, result.second);
                iter = result.second;;
                continue;
            }
            result = reg_symbol_colon(iter, end);
            if(result.first){
                f(token_symbol_colon, iter, result.second);
                iter = result.second;;
                continue;
            }
            result = reg_equal(iter, end);
            if(result.first){
                f(token_equal, iter, result.second);
                iter = result.second;;
                continue;
            }
            result = reg_str(iter, end);
            if(result.first){
                f(token_str, iter, result.second);
                iter = result.second;;
                continue;
            }
            break;
        }
        return result;
    }
};
} } // namespace scanner

#endif // SCANNER_LEXER_HPP_

