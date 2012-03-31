#if    defined(_MSC_VER) && (_MSC_VER >= 1400)
    // !!
    // boost::spirit::qi使用時に出てくる関数に対するconst/volatile修飾子無効の警告を抑制する.
#    pragma warning(disable:4180)
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/arithmetic/mul.hpp>
#include <boost/preprocessor/arithmetic/div.hpp>
#include <boost/range.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include "scanner.hpp"
#include "exception.hpp"

namespace kp19pp{
    namespace scanner{
        term_type epsilon_functor::operator ()() const{
            return 0;
        }

        term_type end_of_seq_functor::operator ()() const{
            return (std::numeric_limits<int>::max)();
        }

        string_iter_pair_type::string_iter_pair_type() :
            base_type()
        {}

        string_iter_pair_type::string_iter_pair_type(const string_iter_pair_type &other) :
            base_type(other)
        {}
        
        string_iter_pair_type::string_iter_pair_type(string_iter_pair_type &&other) :
            base_type(other)
        {}
        
        string_iter_pair_type::string_iter_pair_type(
            scanner_string_type::const_iterator first,
            scanner_string_type::const_iterator last
        ) :
            base_type(first, last)
        {}

        string_iter_pair_type &string_iter_pair_type::operator =(const string_iter_pair_type &other){
            base_type::operator =(other);
            return *this;
        }

        string_iter_pair_type &string_iter_pair_type::operator =(std::pair<scanner_string_type::const_iterator, scanner_string_type::const_iterator> pair){
            base_type::operator =(pair);
            return *this;
        }

        const scanner_string_type dummy_string;
        
        semantic_type::semantic_type() :
            action(&eat)
        {}

        semantic_type::semantic_type(const semantic_type &other) :
            action(other.action)
        {}

        semantic_type::semantic_type(semantic_type &&other) :
            action(std::move(other.action))
        {}

        semantic_type::token_type semantic_type::operator ()(const value_type &value, scanner_type &data) const{
            return action(value, data);
        }

        semantic_type::token_type semantic_type::eat(const value_type &value, scanner_type &data){
            token_type result;
            result.value = std::make_pair(dummy_string.begin(), dummy_string.end());
            result.term = epsilon_functor()();
            return result;
        }

        scanner_type::symbol_type::symbol_type(){}
        scanner_type::symbol_type::symbol_type(const symbol_type &other) :
            value(other.value)
        {}

        bool scanner_type::symbol_type::operator ==(const symbol_type &other) const{
            return value.term == other.value.term;
        }

        std::size_t scanner_type::symbol_type::hash::operator ()(const symbol_type &item) const{
            std::size_t h = 0;
            hash_combine(h, scanner_type::token_type::hash()(item.value));
            return h;
        }

        scanner_type::terminal_symbol_data_type::terminal_symbol_data_type(){}

        scanner_type::terminal_symbol_data_type::terminal_symbol_data_type(const terminal_symbol_data_type &other) :
            type(other.type),
            linkdir(other.linkdir),
            priority(other.priority)
        {}

        scanner_type::nonterminal_symbol_data_type::nonterminal_symbol_data_type(){}
        scanner_type::nonterminal_symbol_data_type::nonterminal_symbol_data_type(const nonterminal_symbol_data_type &other) :
            type(other.type),
            rhs(other.rhs)
        {}

        scanner_type::nonterminal_symbol_data_type::rhs_type::rhs_type() :
            base_type(),
            semantic_action(),
            tag(),
            number(),
            argindex_to_symbol_map(),
            argindex_max(-1)
        {}

        scanner_type::nonterminal_symbol_data_type::rhs_type::rhs_type(const rhs_type &other) :
            base_type(other),
            semantic_action(other.semantic_action),
            tag(other.tag),
            number(other.number),
            argindex_to_symbol_map(other.argindex_to_symbol_map)
        {}

        scanner_type::nonterminal_symbol_data_type::rhs_type::rhs_type(rhs_type &&other) :
            base_type(other),
            semantic_action(std::move(other.semantic_action)),
            tag(std::move(other.tag)),
            number(std::move(other.number)),
            argindex_to_symbol_map(std::move(other.argindex_to_symbol_map))
        {}

        bool scanner_type::nonterminal_symbol_data_type::rhs_type::operator==(const rhs_type &other) const{
            if(size() != other.size()){ return false; }
            for(auto iter = begin(), last = end(), other_iter = other.begin(); iter != last; ++iter, ++other_iter){
                if(iter->first->value.term != other_iter->first->value.term){ return false; }
            }
            return true;
        }

        void scanner_type::nonterminal_symbol_data_type::rhs_type::clear(){
            base_type::clear();
            semantic_action.value = string_iter_pair_type(dummy_string.begin(), dummy_string.end());
        }
            
        std::size_t scanner_type::nonterminal_symbol_data_type::rhs_type::hash::operator ()(const rhs_type &item) const{
            std::size_t h = 0;
            for(auto iter = item.begin(), end = item.end(); iter != end; ++iter){
                hash_combine(h, iter->first->value.term);
            }
            return h;
        }

        scanner_type::scanner_type() :
            analysis_phase(phase_token),
            current_priority(0),
            current_terminal_symbol_id(1),
            current_nonterminal_symbol_id(1),
            current_rhs_number(0)
        {
            start_prime.push_back('S'), start_prime.push_back('\'');
        }

        term_type scanner_type::next_terminal_symbol_id(){
            return current_terminal_symbol_id++;
        }

        term_type scanner_type::next_nonterminal_symbol_id(){
            return current_nonterminal_symbol_id++;
        }

        std::size_t scanner_type::next_rhs_number(){
            return current_rhs_number++;
        }

        void scanner_type::clear_rhs_number(){
            current_rhs_number = 0;
        }

        void scanner_type::check_undefined_nonterminal_symbol(){
            if(undefined_nonterminal_symbol_set.empty()){ return; }
            exception_seq e("定義されていない記号です.");
            for(
                auto iter = undefined_nonterminal_symbol_set.begin(), end = undefined_nonterminal_symbol_set.end();
                iter != end;
                ++iter
            ){
                e.seq.push_back(
                    exception(
                        std::string(iter->value.value.begin(), iter->value.value.end()),
                        iter->value.char_num, iter->value.line_num
                    )
                );
            }
            throw(e);
        }

        void scanner_type::normalize_token_order(){
            if(token_order == order_ascending){ return; }
            std::size_t n = 0;
            for(auto iter = terminal_symbol_map.begin(), end = terminal_symbol_map.end(); iter != end; ++iter){
                std::size_t p = iter->second.priority;
                if(n < p){ n = p; }
            }
            for(auto iter = terminal_symbol_map.begin(), end = terminal_symbol_map.end(); iter != end; ++iter){
                iter->second.priority = n - iter->second.priority;
            }
        }

        term_type dummy_term = (std::numeric_limits<int>::max)() - 1;
        scanner_type scanner;
        std::unordered_map<term_type, std::string> term_to_str_map;
        namespace terminal_symbol{
            namespace aux{
                int current_terminal_symbol_id = 1;
            }
#define DECL(name) \
    scanner_type::term_type name; \
    struct BOOST_PP_CAT(name, _initializer_type){ \
        BOOST_PP_CAT(name, _initializer_type)(){ \
            name = aux::current_terminal_symbol_id++; \
            scanner.add_terminal_symbol(name, scanner_type::symbol_data_type()); \
            term_to_str_map[name] = BOOST_PP_STRINGIZE(name); \
        } \
    } BOOST_PP_CAT(name, _initializer);
            DECL(identifier);
            DECL(value);
            DECL(comma);
            DECL(dot);
            DECL(double_colon);
            DECL(semicolon);
            DECL(l_square_bracket);
            DECL(r_square_bracket);
            DECL(l_curly_bracket);
            DECL(r_curly_bracket);
            DECL(l_bracket);
            DECL(r_bracket);
            DECL(l_round_pare);
            DECL(r_round_pare);
            DECL(symbol_or);
            DECL(symbol_colon);
#undef DECL
            struct epsilon_initializer_type{
                epsilon_initializer_type(){
                    scanner.add_terminal_symbol(epsilon_functor()(), scanner_type::symbol_data_type());
                    term_to_str_map[epsilon_functor()()] = "epsilon";
                }
            } epsilon_initializer;

            struct end_of_seq_initializer_type{
                end_of_seq_initializer_type(){
                    scanner.add_terminal_symbol(end_of_seq_functor()(), scanner_type::symbol_data_type());
                    term_to_str_map[epsilon_functor()()] = "$";
                }
            } end_of_seq_initializer;
        }

        namespace nonterminal_symbol{
            namespace aux{
                int current_nonterminal_symbol_id = 1;
            }

#define DECL(name) \
    scanner_type::expression_type name; \
    struct BOOST_PP_CAT(name, _initializer_type){ \
        BOOST_PP_CAT(name, _initializer_type)(){ \
            name.lhs = -(aux::current_nonterminal_symbol_id++); \
            term_to_str_map[name.lhs] = BOOST_PP_STRINGIZE(name); \
        } \
    } BOOST_PP_CAT(name, _initializer);
            DECL(StartPrime);
            DECL(Start);
            DECL(IdentifierSeq);
            DECL(Type);
            DECL(DoubleColon_opt);
            DECL(NestIdentifier);
            DECL(NestIdentifier_opt);
            DECL(TemplateArg_opt);
            DECL(TypeSeq_opt);
            DECL(TypeSeqRest);
            DECL(SymbolType_opt);
            DECL(LHSType);
            DECL(LinkDir);
            DECL(BlockWithLinkDir);
            DECL(SeqStatements);
            DECL(SeqStatementsElement);
            DECL(TopLevelSeqStatements);
            DECL(TopLevelSeqStatementsElement);
            DECL(Arg_opt);
            DECL(SemanticAction);
            DECL(SemanticActionElement_opt);
            DECL(Tag_opt);
            DECL(RHSSeqElement);
            DECL(RHSSeq);
            DECL(RHSSeq_opt);
            DECL(RHS);
            DECL(LHS);
            DECL(Exp);
            DECL(ExpStatements);
            DECL(ExpStatementsRest);
            DECL(Header);
            DECL(GrammarNamespace);
            DECL(TokenNamespace);
            DECL(TokenBody);
            DECL(GrammarBody);
#undef DECL
        }

        namespace semantic_action{
            typedef scanner_type::token_type token_type;
            typedef scanner_type::semantic_type semantic_type;
            token_type join_token(const token_type &front, const token_type &back){
                token_type t;
                t.value = std::make_pair(front.value.begin(), front.value.end());
                return t;
            }

            scanner_type::symbol_type make_symbol(const token_type &token, term_type term){
                scanner_type::symbol_type symbol;
                symbol.value = token;
                symbol.value.term = term;
                return symbol;
            }

            scanner_type::terminal_symbol_data_type make_terminal_symbol(
                const token_type &symbol_type,
                int priority
            ){
                scanner_type::terminal_symbol_data_type data;
                data.type = symbol_type;
                data.priority = priority;
                return data;
            }

            std::pair<scanner_type::terminal_symbol_map_type::iterator, bool> insert_terminal_symbol(
                scanner_type &data,
                const scanner_type::symbol_type &symbol,
                const scanner_type::terminal_symbol_data_type &identifier
            ){
                auto ret = data.terminal_symbol_map.insert(std::make_pair(symbol, identifier));
                if(!ret.second){
                    throw(
                        exception(
                            "'" +
                            std::string(symbol.value.value.begin(), symbol.value.value.end()) +
                            "' トークンの多重定義です.",
                            symbol.value.char_num,
                            symbol.value.line_num
                        )
                    );
                }
                data.current_terminal_symbol_seq.push_back(&ret.first->second);
                return ret;
            }

            token_type eat(const semantic_type::value_type &value, scanner_type &data){
                return semantic_type::eat(value, data); // auto
            }

            token_type identifier_seq_a(const semantic_type::value_type &value, scanner_type &data){
                auto &identifier(value[0]);
                auto &symbol_type(value[1]);
                insert_terminal_symbol(data, make_symbol(identifier, data.next_terminal_symbol_id()), make_terminal_symbol(symbol_type, data.current_priority));
                return join_token(identifier, symbol_type);
            }

            token_type identifier_seq_b(const semantic_type::value_type &value, scanner_type &data){
                auto &identifier(value[2]);
                auto &symbol_type(value[3]);
                insert_terminal_symbol(data, make_symbol(identifier, data.next_terminal_symbol_id()), make_terminal_symbol(symbol_type, data.current_priority));
                return join_token(identifier, symbol_type);
            }

            token_type make_type(const semantic_type::value_type &value, scanner_type &data){
                auto &double_colon(value[0]);
                auto &identifier(value[1]);
                auto &template_arg(value[2]);
                auto &nest_identifier(value[3]);
                token_type r;
                if(double_colon.value.begin() != double_colon.value.end()){
                    r = double_colon;
                }else{
                    r = identifier;
                }
                if(template_arg.value.begin() != template_arg.value.end()){
                    r = join_token(r, template_arg);
                }
                if(nest_identifier.value.begin() != nest_identifier.value.end()){
                    r = join_token(r, nest_identifier);
                }
                return r;
            }

            token_type make_double_colon(const semantic_type::value_type &vec, scanner_type &data){
                return vec[0];
            }

            token_type make_nest_identifier_opt(const semantic_type::value_type &value, scanner_type &data){
                return value[0];
            }

            token_type make_nest_identifier_a(const semantic_type::value_type &value, scanner_type &data){
                auto &delim(value[0]);
                auto &identifier(value[1]);
                auto &template_arg(value[2]);
                token_type r;
                r = join_token(delim, identifier);
                if(template_arg.value.begin() != template_arg.value.end()){
                    r = join_token(r, template_arg);
                }
                return r;
            }

            token_type make_nest_identifier_b(const semantic_type::value_type &value, scanner_type &data){
                auto &nest_identifier(value[0]);
                auto &delim(value[1]);
                auto &identifier(value[2]);
                auto &template_arg(value[3]);
                token_type r;
                r = join_token(nest_identifier, delim);
                r = join_token(r, identifier);
                if(template_arg.value.begin() != template_arg.value.end()){
                    r = join_token(r, template_arg);
                }
                return r;
            }

            token_type make_template_arg(const semantic_type::value_type &value, scanner_type &data){
                return join_token(value[0], value.back());
            }

            token_type make_type_seq(const semantic_type::value_type &value, scanner_type &data){
                return join_token(value[0], value.back());
            }

            token_type make_symbol_type(const semantic_type::value_type &value, scanner_type &data){
                return value[1];
            }

            token_type make_link_dir(const semantic_type::value_type &value, scanner_type &data){
                return value[1];
            }

            token_type make_block_with_linkdir(const semantic_type::value_type &value, scanner_type &data){
                auto comp = [](string_iter_pair_type pair, const char *str) -> bool{
                    for(; ; ){
                        bool a = pair.begin() == pair.end(), b = !*str;
                        if(a && b){ break; }
                        if(a ^ b || *pair.begin() != *str){ return false; }
                        pair = string_iter_pair_type(pair.begin() + 1, pair.end());
                        ++str;
                    }
                    return true;
                };
                auto &linkdir(value[0]);
                scanner_type::terminal_symbol_linkdir linkdir_value;
                if(linkdir.value.begin() == linkdir.value.end()){
                    linkdir_value = scanner_type::nonassoc;
                }else{
                    if(comp(linkdir.value, "left")){
                        linkdir_value = scanner_type::left;
                    }else if(comp(linkdir.value, "right")){
                        linkdir_value = scanner_type::right;
                    }else if(comp(linkdir.value, "nonassoc")){
                        linkdir_value = scanner_type::nonassoc;
                    }else{
                        throw(
                            exception(
                                "'" +
                                std::string(linkdir.value.begin(), linkdir.value.end()) +
                                "' 終端記号の結合方向が 'left', 'right', 'nonassoc' のいずれでもありません.",
                                linkdir.char_num,
                                linkdir.line_num
                            )
                        );
                    }
                }
                for(auto iter = data.current_terminal_symbol_seq.begin(), end = data.current_terminal_symbol_seq.end(); iter != end; ++iter){
                    (*iter)->linkdir = linkdir_value;
                }
                data.current_terminal_symbol_seq.clear();
                return eat(value, data);
            }

            token_type make_seq_statements_a(const semantic_type::value_type &value, scanner_type &data){
                ++data.current_priority;
                return value[0];
            }

            token_type make_seq_statements_b(const semantic_type::value_type &value, scanner_type &data){
                ++data.current_priority;
                return join_token(value[0], value[1]);
            }

            token_type make_seq_statements_element(const semantic_type::value_type &value, scanner_type &data){
                return value[0];
            }

            token_type make_top_level_seq_statements_a(const semantic_type::value_type &value, scanner_type &data){
                return value[0];
            }

            token_type make_top_level_seq_statements_b(const semantic_type::value_type &value, scanner_type &data){
                return join_token(value[0], value.back());
            }

            token_type make_top_level_seq_statements_element_a(const semantic_type::value_type &value, scanner_type &data){
                for(auto iter = data.current_terminal_symbol_seq.begin(), end = data.current_terminal_symbol_seq.end(); iter != end; ++iter){
                    (*iter)->linkdir = scanner_type::nonassoc;
                }
                data.current_terminal_symbol_seq.clear();
                ++data.current_priority;
                return value[0];
            }

            token_type make_top_level_seq_statements_element_b(const semantic_type::value_type &value, scanner_type &data){
                data.current_terminal_symbol_seq.clear();
                return eat(value, data);
            }

            token_type make_arg(const semantic_type::value_type &value, scanner_type &data){
                return value[1];
            }

            token_type make_semantic_action(const semantic_type::value_type &value, scanner_type &data){
                data.current_rhs.semantic_action = value[1];
                return value[1];
            }

            token_type make_semantic_action_opt(const semantic_type::value_type &value, scanner_type &data){
                return value[0];
            }

            token_type make_tag(const semantic_type::value_type &value, scanner_type &data){
                auto &tag(value[1]);
                if(data.terminal_symbol_map.find(make_symbol(tag, tag.term)) == data.terminal_symbol_map.end()){
                    throw(
                        exception(
                            "'" +
                            std::string(tag.value.begin(), tag.value.end()) +
                            "' タグが終端記号ではありません.",
                            tag.char_num,
                            tag.line_num
                        )
                    );
                }
                data.current_rhs.tag = tag;
                return value[1];
            }

            token_type insert_rhs_element(const token_type &identifier, const token_type &arg, const semantic_type::value_type &value, scanner_type &data){
                scanner_type::terminal_symbol_data_type terminal_symbol_data;
                scanner_type::symbol_type nonterminal_symbol = make_symbol(identifier, identifier.term);
                std::pair<scanner_type::terminal_symbol_map_type::iterator, bool> find_ret_terminal_symbol;
                std::pair<scanner_type::nonterminal_symbol_map_type::iterator, bool> find_ret_nonterminal_symbol;
                find_ret_terminal_symbol.first = data.terminal_symbol_map.find(make_symbol(identifier, identifier.term));
                find_ret_nonterminal_symbol.first = data.nonterminal_symbol_map.find(make_symbol(identifier, identifier.term));
                std::pair<scanner_type::nonterminal_symbol_map_type::iterator, bool> undefined_nonterminal_symbol_iter;
                bool z = true, w = false;
                if(find_ret_terminal_symbol.first == data.terminal_symbol_map.end()){
                    z = find_ret_nonterminal_symbol.first == data.nonterminal_symbol_map.end();
                    if(z){
                        w = true;
                        scanner_type::nonterminal_symbol_data_type nonterminal_symbol_data;
                        scanner_type::symbol_type symbol = make_symbol(identifier, data.next_nonterminal_symbol_id());
                        find_ret_nonterminal_symbol = undefined_nonterminal_symbol_iter =
                            data.nonterminal_symbol_map.insert(std::make_pair(nonterminal_symbol, nonterminal_symbol_data));
                        scanner_type::symbol_type undefined_nonterminal_symbol;
                        undefined_nonterminal_symbol.value = undefined_nonterminal_symbol_iter.first->first.value;
                        data.undefined_nonterminal_symbol_set.insert(undefined_nonterminal_symbol);
                    }
                }
                auto append_arg = [&](const std::string &str_arg_index, const scanner_type::symbol_type &ret_token_symbol) -> void{
                    int arg_index = std::atoi(str_arg_index.c_str());
                    scanner_type::nonterminal_symbol_data_type::rhs_type &rhs(data.current_rhs);
                    if(rhs.argindex_max < arg_index){ rhs.argindex_max = arg_index; }
                    auto ret_argindex_insert = rhs.argindex_to_symbol_map.insert(std::make_pair(arg_index, &ret_token_symbol));
                    if(!ret_argindex_insert.second){
                        throw(
                            exception(
                                "'" +
                                std::string(find_ret_terminal_symbol.first->first.value.value.begin(), find_ret_terminal_symbol.first->first.value.value.end()) +
                                "' 引数が重複しています.",
                                find_ret_terminal_symbol.first->first.value.char_num,
                                find_ret_terminal_symbol.first->first.value.line_num
                            )
                        );
                    }
                };
                if(arg.value.begin() != arg.value.end()){
                    if(find_ret_terminal_symbol.first != data.terminal_symbol_map.end()){
                        auto &type(terminal_symbol_data.type);
                        if(type.value.begin() == type.value.end()){
                            throw(
                                exception(
                                    "'" +
                                    std::string(identifier.value.begin(), identifier.value.end()) +
                                    "' 引数となる記号に型がありません.",
                                    identifier.char_num,
                                    identifier.line_num
                                )
                            );
                        }
                        std::string str_arg_index(arg.value.begin(), arg.value.end());
                        append_arg(str_arg_index, find_ret_terminal_symbol.first->first);
                    }else{
                        auto &ret_token_symbol(find_ret_nonterminal_symbol.first->first);
                        if(arg.value.begin() != arg.value.end()){
                            std::string arg_str_index(arg.value.begin(), arg.value.end());
                            append_arg(arg_str_index, find_ret_nonterminal_symbol.first->first);
                        }
                    }
                    const scanner_type::symbol_type *symbol_identifier;
                    scanner_type::symbol_type symbol_arg;
                    if(w){
                        symbol_identifier = &undefined_nonterminal_symbol_iter.first->first;
                    }else if(z){
                        symbol_identifier = &find_ret_terminal_symbol.first->first;
                    }else{
                        symbol_identifier = &find_ret_nonterminal_symbol.first->first;
                    }
                    symbol_arg.value = arg;
                    data.current_rhs.push_back(std::make_pair(symbol_identifier, static_cast<scanner_type::symbol_type&>(symbol_arg)));
                    return join_token(identifier, arg);
                }else{
                    const scanner_type::symbol_type *symbol_identifier;
                    if(w){
                        symbol_identifier = &undefined_nonterminal_symbol_iter.first->first;
                    }else if(z){
                        symbol_identifier = &find_ret_terminal_symbol.first->first;
                    }else{
                        symbol_identifier = &find_ret_nonterminal_symbol.first->first;
                    }
                    scanner_type::symbol_type dummy_arg;
                    dummy_arg.value = eat(value, data);
                    data.current_rhs.push_back(std::make_pair(symbol_identifier, dummy_arg));
                    return identifier;
                }
            }

            token_type make_rhs_seq(const semantic_type::value_type &value, scanner_type &data){
                return insert_rhs_element(value[0], value[1], value, data);
            }

            token_type make_rhs_seq_last(const semantic_type::value_type &value, scanner_type &data){
                return insert_rhs_element(value[1], value[2], value, data);
            }

            token_type make_rhs_seq_opt(const semantic_type::value_type &value, scanner_type &data){
                return value[0];
            }

            token_type make_rhs(const semantic_type::value_type &value, scanner_type &data){
                data.current_rhs.number = data.next_rhs_number();
                auto ret = data.current_nonterminal_symbol_iter->second.rhs.insert(data.current_rhs);
                auto &rhs(*ret.first);
                if(!ret.second){
                    throw(
                        exception(
                            "規則の多重定義を検出しました.",
                            rhs.front().first->value.char_num,
                            rhs.front().first->value.line_num
                        )
                    );
                }
                if(!rhs.empty()){
                    for(int i = 0, n = rhs.argindex_max; i <= n; ++i){
                        if(rhs.argindex_to_symbol_map.find(i) == rhs.argindex_to_symbol_map.end()){
                            throw(
                                exception(
                                    "規則内での引数が連続していません.",
                                    data.current_nonterminal_symbol_iter->first.value.char_num,
                                    data.current_nonterminal_symbol_iter->first.value.line_num
                                )
                            );
                        }
                    }
                }
                data.current_rhs.clear();
                return eat(value, data);
            }

            token_type make_rhs_rest(const semantic_type::value_type &value, scanner_type &data){
                data.clear_rhs_number();
                return eat(value, data);
            }

            token_type make_lhs(const semantic_type::value_type &value, scanner_type &data){
                auto &identifier(value[0]);
                auto &type(value.back());
                scanner_type::symbol_type undefined_dummy;
                undefined_dummy.value = identifier;
                auto ret = data.undefined_nonterminal_symbol_set.find(undefined_dummy);
                if(ret == data.undefined_nonterminal_symbol_set.end()){
                    scanner_type::symbol_type terminal_symbol = make_symbol(identifier, identifier.term);
                    scanner_type::terminal_symbol_data_type terminal_symbol_data;
                    scanner_type::symbol_type nonterminal_symbol = make_symbol(identifier, data.next_nonterminal_symbol_id());
                    scanner_type::nonterminal_symbol_data_type nonterminal_symbol_data;
                    terminal_symbol.value = identifier;
                    nonterminal_symbol.value = identifier;
                    nonterminal_symbol_data.type = type;
                    if(data.terminal_symbol_map.find(terminal_symbol) != data.terminal_symbol_map.end()){
                        throw(
                            exception(
                                "'" + std::string(identifier.value.begin(), identifier.value.end()) + "' 終端記号と非終端記号が衝突しました.",
                                identifier.char_num,
                                identifier.line_num
                            )
                        );
                    }
                    auto ret = data.nonterminal_symbol_map.insert(std::make_pair(nonterminal_symbol, nonterminal_symbol_data));
                    if(!ret.second){
                        throw(
                            exception(
                                "'" + std::string(identifier.value.begin(), identifier.value.end()) + "' 非終端記号が再定義されました.",
                                identifier.char_num,
                                identifier.line_num
                            )
                        );
                    }
                    data.current_nonterminal_symbol_iter = ret.first;
                }else{
                    auto &undefined_nonterminal_symbol(*ret);
                    scanner_type::symbol_type nonterminal_symbol = make_symbol(identifier, identifier.term);
                    scanner_type::nonterminal_symbol_data_type nonterminal_symbol_data;
                    scanner_type::nonterminal_symbol_map_type::iterator nonterminal_symbol_iter = data.nonterminal_symbol_map.find(nonterminal_symbol);
                    scanner_type::nonterminal_symbol_data_type &deref_nonterminal_symbol(nonterminal_symbol_iter->second);
                    deref_nonterminal_symbol.type = type;
                    data.undefined_nonterminal_symbol_set.erase(ret);
                    data.current_nonterminal_symbol_iter = nonterminal_symbol_iter;
                }
                return join_token(identifier, type);
            }

            token_type make_lhs_type(const semantic_type::value_type &value, scanner_type &data){
                return value[1];
            }

            token_type make_exp(const semantic_type::value_type &value, scanner_type &data){
                data.clear_rhs_number();
                return eat(value, data);
            }

            token_type make_exp_statements_a(const semantic_type::value_type &value, scanner_type &data){
                return eat(value, data);
            }

            token_type make_exp_statements_b(const semantic_type::value_type &value, scanner_type &data){
                return eat(value, data);
            }

            token_type make_header(const semantic_type::value_type &value, scanner_type &data){
                auto &header(value[1]);
                auto str(std::string(header.value.begin(), header.value.end()));
                if(data.analysis_phase == scanner_type::phase_token){
                    if(str == "token_asc"){
                        data.token_order = scanner_type::order_ascending;
                    }else if(str == "token_desc"){
                        data.token_order = scanner_type::order_descending;
                    }else{
                        throw(exception("ヘッダーが 'token_asc', 'token_desc' のいずれでもありません.", header.char_num, header.line_num));
                    }
                }else if(data.analysis_phase == scanner_type::phase_grammar && str != "grammar"){
                    throw(exception("ヘッダーが'grammar'ではありません.", header.char_num, header.line_num));
                }
                return header;
            }

            token_type make_token_body(const semantic_type::value_type &value, scanner_type &data){
                data.analysis_phase = scanner_type::phase_grammar;
                return eat(value, data);
            }

            token_type make_grammar_body(const semantic_type::value_type &value, scanner_type &data){
                data.analysis_phase = scanner_type::phase_end;
                return eat(value, data);
            }

            token_type make_token_namespace(const semantic_type::value_type &value, scanner_type &data){
                auto &namespace_token(value[0]);
                data.namespace_token = namespace_token.value;
                return namespace_token;
            }

            token_type make_grammar_namespace(const semantic_type::value_type &value, scanner_type &data){
                auto &namespace_grammar(value[0]);
                data.namespace_grammar = namespace_grammar.value;
                return namespace_grammar;
            }
        }

        void define_scanner_grammar(){
            using namespace terminal_symbol;
            using namespace nonterminal_symbol;
#define PUSH_BACK(z, n, elem) rhs.push_back(BOOST_PP_SEQ_ELEM(n, elem));
#define DECL(l, seq, sa) \
    { \
        scanner_type::expression_type::rhs_type rhs; \
        BOOST_PP_REPEAT(BOOST_PP_SEQ_SIZE(seq), PUSH_BACK, seq); \
        rhs.semantic.action = &semantic_action::sa; \
        l.rhs.insert(rhs); \
    }
#define WRAPPED_DECL_SEQ(z, n, elem) \
    DECL( \
        BOOST_PP_SEQ_ELEM(0, elem), \
        BOOST_PP_SEQ_ELEM(BOOST_PP_MUL(n, 2), BOOST_PP_SEQ_ELEM(1, elem)), \
        BOOST_PP_SEQ_ELEM(BOOST_PP_INC(BOOST_PP_MUL(n, 2)), BOOST_PP_SEQ_ELEM(1, elem)) \
    )
#define DECL_SEQ(l, seq) BOOST_PP_REPEAT(BOOST_PP_DIV(BOOST_PP_SEQ_SIZE(seq), 2), WRAPPED_DECL_SEQ, (l)(seq))
#define DECL_EPS(l) { scanner_type::expression_type::rhs_type rhs; l.rhs.insert(rhs); }
#define DECL_SEQS(l, seq) DECL_SEQ(l, seq); { scanner.add_expression(l); }
#define DECL_SEQS_EPS(l, seq) DECL_SEQ(l, seq); DECL_EPS(l); { scanner.add_expression(l); }
            DECL_SEQS(
                IdentifierSeq,
                ((identifier)(SymbolType_opt.lhs))                                  (identifier_seq_a)
                ((IdentifierSeq.lhs)(comma)(identifier)(SymbolType_opt.lhs))        (identifier_seq_b)
            );

            DECL_SEQS(
                Type,
                ((DoubleColon_opt.lhs)(identifier)(TemplateArg_opt.lhs)(NestIdentifier_opt.lhs))
                                                                                    (make_type)
            );

            DECL_SEQS_EPS(
                DoubleColon_opt,
                ((double_colon))                                                    (make_double_colon)
            );

            DECL_SEQS_EPS(
                NestIdentifier_opt,
                ((NestIdentifier.lhs))                                              (make_nest_identifier_opt)
            )

            DECL_SEQS(
                NestIdentifier,
                ((double_colon)(identifier)(TemplateArg_opt.lhs))                    (make_nest_identifier_a)
                ((dot)(identifier)(TemplateArg_opt.lhs))                            (make_nest_identifier_a)
                ((NestIdentifier.lhs)(double_colon)(identifier)(TemplateArg_opt.lhs))
                                                                                    (make_nest_identifier_b)
                ((NestIdentifier.lhs)(dot)(identifier)(TemplateArg_opt.lhs))        (make_nest_identifier_b)
            );

            DECL_SEQS_EPS(
                TemplateArg_opt,
                ((l_bracket)(TypeSeq_opt.lhs)(r_bracket))                           (make_template_arg)
            );

            DECL_SEQS_EPS(
                TypeSeq_opt,
                ((Type.lhs))                                                        (make_type_seq)
                ((Type.lhs)(TypeSeqRest.lhs))                                       (make_type_seq)
            );

            DECL_SEQS(
                TypeSeqRest,
                ((comma)(Type.lhs))                                                 (make_type_seq)
                ((TypeSeqRest.lhs)(comma)(Type.lhs))                                (make_type_seq)
            );

            DECL_SEQS_EPS(
                SymbolType_opt,
                ((l_bracket)(Type.lhs)(r_bracket))                                  (make_symbol_type)
            );

            DECL_SEQS(
                LinkDir,
                ((l_bracket)(identifier)(r_bracket))                                (make_link_dir)
            );

            DECL_SEQS(
                BlockWithLinkDir,
                ((LinkDir.lhs)(l_curly_bracket)(SeqStatements.lhs)(r_curly_bracket))(make_block_with_linkdir)
            );

            DECL_SEQS(
                SeqStatements,
                ((SeqStatementsElement.lhs)(semicolon))                             (make_seq_statements_a)
                ((semicolon))                                                       (eat)
                ((SeqStatements.lhs)(SeqStatementsElement.lhs)(semicolon))          (make_seq_statements_b)
                ((SeqStatements.lhs)(semicolon))                                    (eat)
            );

            DECL_SEQS(
                SeqStatementsElement,
                ((IdentifierSeq.lhs))                                               (make_seq_statements_element)
            );

            DECL_SEQS(
                TopLevelSeqStatements,
                ((TopLevelSeqStatementsElement.lhs))                                (make_top_level_seq_statements_a)
                ((semicolon))                                                       (eat)
                ((TopLevelSeqStatements.lhs)(TopLevelSeqStatementsElement.lhs))     (make_top_level_seq_statements_b)
                ((TopLevelSeqStatements.lhs)(semicolon))                            (eat)
            );

            DECL_SEQS(
                TopLevelSeqStatementsElement,
                ((IdentifierSeq.lhs)(semicolon))                                    (make_top_level_seq_statements_element_a)
                ((BlockWithLinkDir.lhs))                                            (make_top_level_seq_statements_element_b)
            );

            DECL_SEQS_EPS(
                Arg_opt,
                ((l_round_pare)(value)(r_round_pare))                               (make_arg)
            );

            DECL_SEQS(
                SemanticAction,
                ((l_square_bracket)(SemanticActionElement_opt.lhs)(r_square_bracket))
                                                                                    (make_semantic_action)
            );

            DECL_SEQS_EPS(
                SemanticActionElement_opt,
                ((identifier))                                                      (make_semantic_action_opt)
            );

            DECL_SEQS_EPS(
                Tag_opt,
                ((l_bracket)(identifier)(r_bracket))                                (make_tag)
            );

            DECL_SEQS(
                RHSSeq,
                ((identifier)(Arg_opt.lhs))                                         (make_rhs_seq)
                ((RHSSeq.lhs)(identifier)(Arg_opt.lhs))                             (make_rhs_seq_last)
            );

            DECL_SEQS_EPS(
                RHSSeq_opt,
                ((RHSSeq.lhs))                                                      (make_rhs_seq_opt)
            );

            DECL_SEQS(
                RHS,
                ((SemanticAction.lhs)(Tag_opt.lhs)(RHSSeq_opt.lhs))                 (make_rhs)
                ((RHS.lhs)(symbol_or)(SemanticAction.lhs)(Tag_opt.lhs)(RHSSeq_opt.lhs))
                                                                                    (make_rhs)
            );

            DECL_SEQS(
                LHS,
                ((identifier)(LHSType.lhs))                                         (make_lhs)
            );

            DECL_SEQS(
                LHSType,
                ((l_bracket)(Type.lhs)(r_bracket))                                  (make_lhs_type)
            );

            DECL_SEQS(
                Exp,
                ((LHS.lhs)(symbol_colon)(RHS.lhs))                                  (make_exp)
            );

            DECL_SEQS(
                ExpStatements,
                ((Exp.lhs)(semicolon))                                              (make_exp_statements_a)
                ((semicolon))                                                       (eat)
                ((ExpStatements.lhs)(Exp.lhs)(semicolon))                           (make_exp_statements_b)
                ((ExpStatements.lhs)(semicolon))                                    (eat)
            );

            DECL_SEQS(
                Header,
                ((l_bracket)(identifier)(r_bracket))                                (make_header)
            );

            DECL_SEQS(
                TokenBody,
                ((l_curly_bracket)(TopLevelSeqStatements.lhs)(r_curly_bracket))     (make_token_body)
            );
            
            DECL_SEQS(
                GrammarBody,
                ((l_curly_bracket)(ExpStatements.lhs)(r_curly_bracket))             (make_grammar_body)
            );

            DECL_SEQS_EPS(
                TokenNamespace,
                ((identifier))                                                      (make_token_namespace)
            );

            DECL_SEQS(
                GrammarNamespace,
                ((identifier))                                                      (make_grammar_namespace)
            );

            DECL_SEQS(
                Start,
                ((Header.lhs)(TokenNamespace.lhs)(TokenBody.lhs)(Header.lhs)(GrammarNamespace.lhs)(GrammarBody.lhs))
                                                                                    (eat)
            );

            DECL_SEQS(
                StartPrime,
                ((Start.lhs))                                                       (eat)
            );
#undef PUSH_BACK
#undef DECL
#undef WRAPPED_DECL_SEQ
#undef DECL_SEQ
#undef DECL_EPS
#undef DECL_SEQS
#undef DECL_SEQS_EPS
        }

        // !!
        // マルチスレッド未対応
        class lexer{
        public:
            lexer(scanner_type::token_seq_type &token_seq_){
                resert(&token_seq_);
            }

            void resert(scanner_type::token_seq_type *token_seq_){
                char_count() = 0;
                line_count() = 0;
                token_seq() = token_seq_;
            }

        private:
#define F(name) \
    static void f_ ## name( \
        const boost::iterator_range<scanner_string_type::const_iterator> &range, \
        const boost::spirit::qi::unused_type&, \
        bool \
    ){ \
        token_seq()->push_back(scanner_type::token_type(string_iter_pair_type(range.begin(), range.end()), terminal_symbol::name, char_count(), line_count())); \
        char_count() += range.size(); \
    }
            F(identifier);
            F(value);
            F(comma);
            F(dot);
            F(double_colon);
            F(semicolon);
            F(l_square_bracket);
            F(r_square_bracket);
            F(l_curly_bracket);
            F(r_curly_bracket);
            F(l_bracket);
            F(r_bracket);
            F(l_round_pare);
            F(r_round_pare);
            F(symbol_or);
            F(symbol_colon);
#undef F
            static void f_whitespace(
                const boost::iterator_range<scanner_string_type::const_iterator> &range,
                const boost::spirit::qi::unused_type&,
                bool
            ){
                char_count() += range.size();
            }

            static void f_end_of_line(
                const boost::iterator_range<scanner_string_type::const_iterator> &range,
                const boost::spirit::qi::unused_type&,
                bool
            ){
                char_count() = 0;
                ++line_count();
            }

            static void f_c_style_comment(
                const boost::iterator_range<scanner_string_type::const_iterator> &range,
                const boost::spirit::qi::unused_type&,
                bool
            ){
                for(auto iter = range.begin(), end = range.end(); iter != end; ++iter){
                    auto c(*iter);
                    if(c == '\n'){
                        char_count() = 0;
                        ++line_count();
                    }else{
                        ++char_count();
                    }
                }
            }

        public:
            void tokenize(std::istream &in, scanner_string_type &str){
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
                        raw[char_('/') >> char_('/') >> (char_ - char_('\n') >> char_('\n'))][f_end_of_line] |
                        raw[char_('/') >> char_('*') >> *char_ >> char_('*') >> char_('/')][f_c_style_comment] |
                        raw[(char_('a', 'z') | char_('A', 'Z') | char_('_')) >> *(char_('a', 'z') | char_('A', 'Z') | char_('0', '9') | char_('_'))][f_identifier] |
                        raw[(char_('1', '9') >> *char_('0', '9')) | char_('0')][f_value] |
                        raw[char_(',')][f_comma] |
                        raw[char_('.')][f_dot] |
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
                    )){
                        throw(exception("字句解析エラー.", char_count(), line_count()));
                    }
                }
            }

        private:
            static std::size_t &char_count(){
                static std::size_t value;
                return value;
            }
            
            static std::size_t &line_count(){
                static std::size_t value;
                return value;
            }
            
            static scanner_type::token_seq_type *&token_seq(){
                static scanner_type::token_seq_type *ptr;
                return ptr;
            }
        };

        void scanner_type::define_grammar(){
            static bool flag = true;
            if(flag){
                define_scanner_grammar();
                flag = false;
            }
        }

        template<class PutFn, class ErrorFn>
        bool scanner_type::parse(const PutFn &put_fn, const ErrorFn &error_fn){
            typedef std::vector<term_type>   term_stack_type;
            typedef std::vector<std::size_t> state_stack_type;
            typedef std::vector<token_type>  value_stack_type;
            term_stack_type  term_stack;
            state_stack_type state_stack;
            value_stack_type value_stack;
            state_stack.push_back(0);
            for(token_seq_type::const_iterator iter = token_seq.begin(); ; ){
                auto &t(*iter);
                auto &a(t.term);
                std::size_t s = state_stack.back();
                auto &actions(*ref_parsing_table[s].actions);
                auto find_result = actions.find(a);
                if(find_result == actions.end()){
                    error_fn(iter);
                    return false;
                }
                auto &action(find_result->second);
                if(action.first == action_shift){
                    term_stack.push_back(a);
                    state_stack.push_back(action.second);
                    value_stack.push_back(t);
                    ++iter;
                }else if(action.first == action_reduce){
                    std::size_t norm = action.item->rhs->size();
                    term_stack.resize(term_stack.size() - norm);
                    state_stack.resize(state_stack.size() - norm);
                    if(state_stack.size() == 0){
                        error_fn(iter);
                        return false;
                    }
                    value_stack_type arg(value_stack.begin() + (value_stack.size() - norm), value_stack.end());
                    auto ret_value = action.item->rhs->semantic.action(arg, *this);
                    value_stack.resize(value_stack.size() - norm);
                    std::size_t s_prime = state_stack.back();
                    const term_type &lhs(action.item->lhs);
                    term_stack.push_back(lhs);
                    state_stack.push_back(ref_parsing_table[s_prime].goto_fns->find(lhs)->second);
                    value_stack.push_back(ret_value);
                    put_fn(*action.item);
                }else if(action.first == action_acc){
                    break;
                }
            }
            return true;
        }

        void scanner_type::scan(std::istream &in){
            define_grammar();
            lexer lex(token_seq);
            lex.tokenize(in, string);
            token_seq.push_back(
                token_type(
                    string_iter_pair_type(string.end(), string.end()),
                    end_of_seq_functor()(),
                    token_seq.back().char_num + 1,
                    token_seq.back().line_num
                )
            );

            auto is_not_terminal = [](const expression_set_type&, term_type id) -> bool{
                return id < 0;
            };

            auto term_to_str = [](term_type id) -> std::string{
                return term_to_str_map[id];
            };

            make_parsing_table_option option;
            // option.put_log = true; // デバッグ用

            bool result = make_parsing_table(
                nonterminal_symbol::StartPrime,
                dummy_term,
                option,
                is_not_terminal,
                term_to_str
            );

            if(!result){ // !! ブート用の文法が間違っている
                return;
            }

            {
                // ログを残す
                std::ofstream parsing_log("parsing_log.txt");
                auto put_fn = [&](const item_type &item) -> void{
                    parsing_log << "reduce " << term_to_str(item.lhs) << " :";
                    for(auto term_iter = item.rhs->begin(), term_end = item.rhs->end(); term_iter != term_end; ++term_iter){
                        parsing_log << " " << term_to_str(*term_iter);
                    }
                    parsing_log << "\n";
                };

                auto error_fn = [&](token_seq_type::const_iterator iter) -> void{
                    parsing_log
                        << "error : "
                        << iter->line_num + 1
                        << ", "
                        << iter->char_num + 1
                        << ", "
                        << term_to_str(iter->term)
                        << "\n";
                    throw(exception("grammar error. '" + term_to_str(iter->term) + "'.", iter->char_num, iter->line_num));
                };

                /*
                //ログを残さない
                auto put_fn = [](const traits::item_type&) -> void{};
                auto error_fn = [](const token_seq_type::iterator &iter) -> void{ throw(exception("parsing error.", iter->char_num, iter->line_num)); };
                */

                parse(put_fn, error_fn);
            }

            check_undefined_nonterminal_symbol();
            normalize_token_order();
        }
    }
}

