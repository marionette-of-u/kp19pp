#if    defined(_MSC_VER) && (_MSC_VER >= 1400)
    // !!
    // boost::spirit::qi使用時に出てくる関数に対するconst/volatile修飾子無効の警告を抑制する.
#    pragma warning(disable:4180)
#endif

#include <boost/range.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
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

#define DECL(name) \
    void f_ ## name( \
        const boost::iterator_range<scanner_string_type::const_iterator> &range, \
        const boost::spirit::qi::unused_type&, \
        bool \
    ){ \
        lexer::token_seq()->push_back(scanner_type::token_type(string_iter_pair_type(range.begin(), range.end()), terminal_symbol::name, lexer::char_count(), lexer::line_count())); \
        lexer::char_count() += range.size(); \
    }
        KP19PP_SCANNER_DECL_TERMINAL_SYMBOLS();
#undef DECL

        void f_whitespace(
            const boost::iterator_range<scanner_string_type::const_iterator> &range,
            const boost::spirit::qi::unused_type&,
            bool
        ){ lexer::char_count() += range.size(); }

        void f_end_of_line(
            const boost::iterator_range<scanner_string_type::const_iterator> &range,
            const boost::spirit::qi::unused_type&,
            bool
        ){
            lexer::char_count() = 0;
            ++lexer::line_count();
        }

        void lexer::tokenize(std::istream &in, scanner_string_type &str){
            using namespace boost::spirit;
            using namespace boost::spirit::qi;
            {
                std::string str_;
                std::getline(in, str_, '\0');
                str.reserve(str_.size());
                str.assign(str_.begin(), str_.end());
            }
            for(auto iter = str.begin(), end = str.end(); iter != end; ){
                if(!parse(
                    iter, end,
                    raw[+(char_(' ') | char_('\t'))][f_whitespace] |
                    raw[char_('\n')][f_end_of_line] |
                    raw[char_('/') >> char_('/') >> *(char_ - char_('\n')) >> char_('\n')][f_end_of_line] |
                    raw[(char_('a', 'z') | char_('A', 'Z') | char_('_')) >> *(char_('a', 'z') | char_('A', 'Z') | char_('0', '9') | char_('_'))][f_identifier] |
                    raw[(char_('1', '9') >> *char_('0', '9')) | char_('0')][f_value] |
                    raw[char_(',')][f_comma] |
                    raw[char_('.')][f_dot] |
                    raw[char_('*')][f_asterisk] |
                    raw[char_('&')][f_ampersand] |
                    raw[char_(':') >> char_(':')][f_double_colon] |
                    raw[char_(';')][f_semicolon] |
                    raw[char_('[')][f_l_square_bracket] |
                    raw[char_(']')][f_r_square_bracket] |
                    raw[char_('{')][f_l_curly_bracket] |
                    raw[char_('}')][f_r_curly_bracket] |
                    raw[char_('<')][f_l_bracket] |
                    raw[char_('>')][f_r_bracket] |
                    raw[char_('(')][f_l_round_pare] |
                    raw[char_(')')][f_r_round_pare] |
                    raw[char_('|')][f_symbol_or] |
                    raw[char_(':')][f_symbol_colon]
                )){ throw(exception("字句解析エラー.", char_count(), line_count())); }
            }
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
    }
}
