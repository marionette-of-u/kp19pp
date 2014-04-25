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
#include "scanner_lexer.hpp"
#include "scanner.hpp"
#include "exception.hpp"

namespace kp19pp{
    namespace scanner{
        exception make_exception(const scanner_type &data, const std::string &str, const scanner_type::token_type::value_type &t){
            exception e(str, 0, 0);
            auto storage_first = data.ref_string.begin(), storage_last = data.ref_string.end();
            --storage_last;
            if(&*t.begin() > &*storage_first && &*t.begin() < &*storage_last){
                utf8::iterator<scanner_type::token_type::value_type::const_iterator>
                    iter(t.begin(), data.ref_string.begin(), data.ref_string.end()),
                    jter(data.ref_string.begin(), data.ref_string.begin(), data.ref_string.end());
                while(jter != iter){ ++jter; }
                e.line_num = jter.ref_line_count;
                e.char_num = jter.ref_char_count;
            }
            return e;
        }

        term_type epsilon_functor::operator ()() const{
            return 0;
        }

        term_type end_of_seq_functor::operator ()() const{
            return (std::numeric_limits<int>::max)();
        }

        template<class UnusedType>
        bool is_not_terminal_functor::operator ()(const UnusedType&, term_type term) const{
            return term < 0;
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

        semantic_type &semantic_type::operator =(const semantic_type &other){
            action = other.action;
            return *this;
        }

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
        scanner_type::symbol_type::symbol_type(const token_type &token) : value(token){}
        scanner_type::symbol_type::symbol_type(const symbol_type &other) :
            value(other.value)
        {}

        bool scanner_type::symbol_type::operator ==(const symbol_type &other) const{
            auto iter = value.value.begin(), other_iter = other.value.value.begin(), end = value.value.end();
            for(; iter != end; ++iter, ++other_iter){
                if(*iter != *other_iter){
                    return false;
                }
            }
            if(other_iter != other.value.value.end()){
                return false;
            }
            return true;
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

        scanner_type::nonterminal_symbol_data_type::nonterminal_symbol_data_type(nonterminal_symbol_data_type &&other) :
            type(std::move(other.type)),
            rhs(std::move(other.rhs))
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
            argindex_to_symbol_map(other.argindex_to_symbol_map),
            argindex_max(other.argindex_max)
        {}

        scanner_type::nonterminal_symbol_data_type::rhs_type::rhs_type(rhs_type &&other) :
            base_type(other),
            semantic_action(std::move(other.semantic_action)),
            tag(std::move(other.tag)),
            number(std::move(other.number)),
            argindex_to_symbol_map(std::move(other.argindex_to_symbol_map)),
            argindex_max(std::move(other.argindex_max))
        {}

        bool scanner_type::nonterminal_symbol_data_type::rhs_type::operator==(const rhs_type &other) const{
            if(size() != other.size()){ return false; }
            for(auto iter = begin(), last = end(), other_iter = other.begin(); iter != last; ++iter, ++other_iter){
                if(!(iter->first.value.value == other_iter->first.value.value)){ return false; }
            }
            return true;
        }

        void scanner_type::nonterminal_symbol_data_type::rhs_type::clear(){
            base_type::clear();
            semantic_action.value = string_iter_pair_type(dummy_string.begin(), dummy_string.end());
            tag.value = string_iter_pair_type(dummy_string.begin(), dummy_string.end());
            tag.term = epsilon_functor()();
            number = 0;
            argindex_to_symbol_map.clear();
            argindex_max = -1;
        }
            
        std::size_t scanner_type::nonterminal_symbol_data_type::rhs_type::hash::operator ()(const rhs_type &item) const{
            std::size_t h = 0;
            for(auto iter = item.begin(), end = item.end(); iter != end; ++iter){
                hash_combine(h, iter->first.value.term);
            }
            return h;
        }

        scanner_type::scanner_type() :
            analysis_phase(phase_token),
            current_priority(0),
            current_token_number(0),
            current_terminal_symbol_id(1),
            current_nonterminal_symbol_id(2),
            current_rhs_number(0),
            current_rhs_arg_number(0),
            external_token(false),
            scanned_first_nonterminal_symbol(false)
        {}

        void scanner_type::clear_extended_data(){
            extended_data_stack.clear();
        }

        scanner_type::extended_data_type &scanner_type::current_extended_data(){
            return extended_data_stack.back();
        }

        bool scanner_type::current_extended_data_is_empty() const{
            return extended_data_stack.empty();
        }

        void scanner_type::pop_current_extended_data(){
            extended_data_stack.pop_back();
        }

        scanner_type::extended_data_type &scanner_type::push_extended_data(
            const token_type &identifier,
            const std::vector<token_type> &token_seq,
            const token_type &semantic_action,
            const token_type &decl,
            const token_type &type
        ){
            extended_data_type x;
            x.identifier = identifier;
            x.token_seq = token_seq;
            x.semantic_action = semantic_action;
            x.decl = decl;
            x.type = type;
            extended_data_stack.push_back(x);
            return extended_data_stack.back();
        }

        bool scanner_type::extended_rule_is_scanned(const token_type &token){
            auto iter = scanned_declared_extended_set.find(token);
            if(iter == scanned_declared_extended_set.end()){
                scanned_declared_extended_set.insert(token);
                return false;
            }
            return true;
        }

        void scanner_type::group_seq_type::clear(){
            seq.clear();
            head_is_scanned = false;
        }

        void scanner_type::push_group_seq_element(const token_type &token, const token_type &arg){
            if(!current_group_seq.head_is_scanned){
                current_group_seq.head_is_scanned = true;
                if(!current_group_seq.seq.empty()){
                    group_seq_stack.push_back(current_group_seq);
                    current_group_seq.clear();
                }
            }
            current_group_seq.seq.push_back(std::make_pair(token, arg));
        }

        void scanner_type::register_group_seq(){
            group_seq_stack.push_back(current_group_seq);
            current_group_seq.head_is_scanned = false;
            current_group_seq.clear();
        }

        void scanner_type::dispose_group_seq(){
            current_group_seq.head_is_scanned = false;
            current_group_seq.clear();
        }

        scanner_type::token_type scanner_type::add_extended_rule_name(const token_type &token, char c){
            scanner_string_type str(token.value.begin(), token.value.end());
            str.push_back(c);
            extended_storage.push_front(str);
            token_type ret_token;
            ret_token.term = token_identifier;
            ret_token.value = string_iter_pair_type(extended_storage.front().begin(), extended_storage.front().end());
            return ret_token;
        }

        scanner_type::token_type scanner_type::add_extended_rule_name(const token_type &token, std::size_t n){
            std::string value = lexical_cast(n);
            scanner_string_type str(token.value.begin(), token.value.end());
            value.push_back('#');
            for(const auto c : value){ str.push_back(c); }
            extended_storage.push_front(str);
            token_type ret_token;
            ret_token.term = token_identifier;
            ret_token.value = string_iter_pair_type(extended_storage.front().begin(), extended_storage.front().end());
            return ret_token;
        }

        scanner_type::nonterminal_symbol_data_type &scanner_type::make_nonterminal_symbol(token_type &token, const token_type &nonterminal_type){
            scanner_type::symbol_type nonterminal_symbol;
            token.term = next_nonterminal_symbol_id();
            nonterminal_symbol.value = token;
            nonterminal_symbol_data_type nonterminal_symbol_data;
            nonterminal_symbol_data.type = nonterminal_type;
            auto r = nonterminal_symbol_map.insert(std::make_pair(nonterminal_symbol, nonterminal_symbol_data));
            return r.first->second;
        }

        term_type scanner_type::next_terminal_symbol_id(){
            term_type r = current_terminal_symbol_id;
            ++current_terminal_symbol_id;
            return r;
        }

        term_type scanner_type::next_nonterminal_symbol_id(){
            return -(current_nonterminal_symbol_id++);
        }

        std::size_t scanner_type::next_rhs_number(){
            return current_rhs_number++;
        }

        std::size_t scanner_type::next_rhs_arg_number(){
            return current_rhs_arg_number;
        }

        void scanner_type::inc_current_rhs_arg_number(){
            ++current_rhs_arg_number;
        }

        void scanner_type::clear_current_rhs_arg_number(){
            current_rhs_arg_number = 0;
        }

        void scanner_type::clear_rhs_number(){
            current_rhs_number = 0;
        }

        bool scanner_type::get_scanned_first_nonterminal_symbol() const{
            return scanned_first_nonterminal_symbol;
        }

        void scanner_type::set_scanned_first_nonterminal_symbol(){
            scanned_first_nonterminal_symbol = true;
        }

        void scanner_type::check_undefined_nonterminal_symbol(){
            if(undefined_nonterminal_symbol_set.empty()){ return; }
            exception_seq e("未定義の記号です.");
            for(
                auto iter = undefined_nonterminal_symbol_set.begin(), end = undefined_nonterminal_symbol_set.end();
                iter != end;
                ++iter
            ){
                e.seq.push_back(make_exception(*this, iter->value.value.to_string(), iter->value.value));
            }
            throw(e);
        }

        void scanner_type::check_linked_nonterminal_symbol(){
            std::set<term_type> scanned_nonterminal_symbol_set;
            std::function<void(term_type, const nonterminal_symbol_data_type*)> rec_fn;
            rec_fn = [&](term_type term, const nonterminal_symbol_data_type *data) -> void{
                if(!scanned_nonterminal_symbol_set.insert(term).second){
                    return;
                }
                for(auto iter = data->rhs.begin(), end = data->rhs.end(); iter != end; ++iter){
                    auto &rhs_seq(*iter);
                    for(auto rhs_iter = rhs_seq.begin(), rhs_end = rhs_seq.end(); rhs_iter != rhs_end; ++rhs_iter){
                        auto &symbol(rhs_iter->first);
                        auto nonterminal_data_iter = nonterminal_symbol_map.find(symbol);
                        if(nonterminal_data_iter == nonterminal_symbol_map.end()){
                            continue;
                        }
                        rec_fn(symbol.value.term, &nonterminal_data_iter->second);
                    }
                }
            };
            rec_fn(
                first_nonterminal_symbol.value.term,
                &nonterminal_symbol_map[first_nonterminal_symbol]
            );
            exception_seq e("使用されない非終端記号です.");
            for(auto iter = nonterminal_symbol_map.begin(), end = nonterminal_symbol_map.end(); iter != end; ++iter){
                auto term(iter->first.value.term);
                if(scanned_nonterminal_symbol_set.find(term) == scanned_nonterminal_symbol_set.end()){
                    e.seq.push_back(make_exception(*this, iter->first.value.value.to_string(), iter->first.value.value));
                }
            }
            if(!e.seq.empty()){
                throw(e);
            }
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

        void scanner_type::collect_token(){
            for(auto iter = terminal_symbol_map.begin(), end = terminal_symbol_map.end(); iter != end; ++iter){
                term_to_token_map[iter->first.value.term] = iter->first.value;
            }
            for(auto iter = nonterminal_symbol_map.begin(), end = nonterminal_symbol_map.end(); iter != end; ++iter){
                term_to_token_map[iter->first.value.term] = iter->first.value;
            }
        }

        void scanner_type::caching_arg_type() const{
            for(
                auto iter = nonterminal_symbol_map.begin(), end = nonterminal_symbol_map.end();
                iter != end;
                ++iter
            ){
                auto &data(iter->second);
                for(
                    auto rhs_iter = data.rhs.begin(), rhs_end = data.rhs.end();
                    rhs_iter != rhs_end;
                    ++rhs_iter
                ){
                    auto &rhs(*rhs_iter);
                    for(
                        auto elem_iter = rhs.argindex_to_symbol_map.begin(), elem_end = rhs.argindex_to_symbol_map.end();
                        elem_iter != elem_end;
                        ++elem_iter
                    ){
                        auto &arg_data(elem_iter->second);
                        if(is_not_terminal_functor()(int(), arg_data.symbol.value.term)){
                            arg_data.type_cache = &nonterminal_symbol_map.find(arg_data.symbol)->second.type;
                        }else{
                            arg_data.type_cache = &terminal_symbol_map.find(arg_data.symbol)->second.type;
                        }
                    }
                }
            }
        }

        void scanner_type::augment(){
            symbol_type start_prime, start;
            start_prime.value.term = -1;
            start.value.term = -2;
            nonterminal_symbol_data_type::rhs_type rhs;
            rhs.push_back(std::make_pair(start, symbol_type()));
            nonterminal_symbol_data_type data;
            data.rhs.insert(rhs);
            nonterminal_symbol_map.insert(std::make_pair(start_prime, data));
            term_to_token_map.insert(std::make_pair(start_prime.value.term, token_type()));
        }

        term_type dummy_term = (std::numeric_limits<int>::max)() - 1;
        std::unordered_map<term_type, std::string> term_to_str_map;
        namespace terminal_symbol{
            namespace aux{
                int current_terminal_symbol_id = 1;
                std::vector<term_type> terminal_symbol_data;
            }
#define DECL(name) \
    struct BOOST_PP_CAT(name, _initializer_type){ \
        BOOST_PP_CAT(name, _initializer_type)(){ \
            name = aux::current_terminal_symbol_id++; \
            aux::terminal_symbol_data.push_back(name); \
            term_to_str_map[name] = BOOST_PP_STRINGIZE(name); \
        } \
    } BOOST_PP_CAT(name, _initializer);
            KP19PP_SCANNER_DECL_TERMINAL_SYMBOLS();
#undef DECL
            struct epsilon_initializer_type{
                epsilon_initializer_type(){
                    aux::terminal_symbol_data.push_back(epsilon_functor()());
                    term_to_str_map[epsilon_functor()()] = "epsilon";
                }
            } epsilon_initializer;

            struct end_of_seq_initializer_type{
                end_of_seq_initializer_type(){
                    aux::terminal_symbol_data.push_back(end_of_seq_functor()());
                    term_to_str_map[epsilon_functor()()] = "$";
                }
            } end_of_seq_initializer;
        }

        namespace semantic_action{
            typedef scanner_type::token_type token_type;
            typedef scanner_type::semantic_type semantic_type;
            token_type join_token(const token_type &front, const token_type &back){
                token_type t = front;
                t.value = std::make_pair(front.value.begin(), back.value.end());
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

            token_type eat(const semantic_type::value_type &value, scanner_type &data){
                return semantic_type::eat(value, data); // auto
            }

            std::pair<scanner_type::terminal_symbol_map_type::iterator, bool> insert_terminal_symbol(
                scanner_type &data,
                const scanner_type::symbol_type &symbol,
                const scanner_type::terminal_symbol_data_type &identifier
            ){
                auto ret = data.terminal_symbol_map.insert(std::make_pair(symbol, identifier));
                if(!ret.second){
                    throw(make_exception(data, "'" + symbol.value.value.to_string() + "' 終端記号が複数回定義されています.", symbol.value.value));
                }
                data.number_to_token_map[data.current_token_number++] = symbol.value;
                data.current_terminal_symbol_seq.push_back(&ret.first->second);
                return ret;
            }

            const scanner_type::nonterminal_symbol_data_type::rhs_type &insert_rhs(
                scanner_type &data,
                scanner_type::nonterminal_symbol_data_type::rhs_type &current_rhs,
                scanner_type::nonterminal_symbol_data_type &current_nonterminal_symbol_data,
                bool bypass_duplicate = false
            ){
                if(current_rhs.empty() && !current_rhs.semantic_action.value.empty()){
                    throw(make_exception(data, "空の生成規則がセマンティックアクションを持っています.", current_rhs.semantic_action.value));
                }
                current_rhs.number = data.next_rhs_number();
                auto ret = current_nonterminal_symbol_data.rhs.insert(current_rhs);
                auto &rhs(*ret.first);
                if(!bypass_duplicate && !ret.second){
                    throw(data, "生成規則が重複しています.", current_rhs.front().first.value);
                }
                return rhs;
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

            token_type make_non_delim_identifier_seq_a(const semantic_type::value_type &value, scanner_type &data){
                return value[0];
            }

            token_type make_non_delim_identifier_seq_b(const semantic_type::value_type &value, scanner_type &data){
                auto &identifier_a(value[0]);
                auto &identifier_b(value[1]);
                return join_token(identifier_a, identifier_b);
            }

            token_type make_non_delim_identifier_seq_c(const semantic_type::value_type &value, scanner_type &data){
                return value[0];
            }

            token_type make_type_a(const semantic_type::value_type &value, scanner_type &data){
                auto &double_colon(value[0]);
                auto &identifier(value[1]);
                auto &template_arg(value[2]);
                auto &nest_identifier(value[3]);
                token_type r;
                if(!double_colon.value.empty()){
                    r = double_colon;
                }else{
                    r = identifier;
                }
                if(!template_arg.value.empty()){
                    r = join_token(r, template_arg);
                }
                if(!nest_identifier.value.empty()){
                    r = join_token(r, nest_identifier);
                }
                return r;
            }

            token_type make_type_b(const semantic_type::value_type &value, scanner_type &data){
                auto &type(value[0]);
                auto &specifier(value[1]);
                return join_token(type, specifier);
            }

            token_type make_reference_specifier(const semantic_type::value_type &value, scanner_type &data){
                return value[0];
            }

            token_type make_reference_specifier_opt(const semantic_type::value_type &value, scanner_type &data){
                return value[0];
            }

            token_type make_double_colon_opt(const semantic_type::value_type &value, scanner_type &data){
                return value[0];
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
                if(!template_arg.value.empty()){
                    r = join_token(r, template_arg);
                }
                return r;
            }

            token_type make_nest_identifier_b(const semantic_type::value_type &value, scanner_type &data){
                auto &delim(value[0]);
                auto &identifier(value[1]);
                auto &template_arg(value[2]);
                auto &nest_identifier_rest(value[3]);
                token_type r;
                r = join_token(delim, identifier);
                if(!template_arg.value.empty()){
                    r = join_token(r, template_arg);
                }
                if(!nest_identifier_rest.value.empty()){
                    r = join_token(r, nest_identifier_rest);
                }
                return r;
            }

            token_type make_template(const semantic_type::value_type &value, scanner_type &data){
                auto &l_bracket(value[0]);
                auto &arguments(value[1]);
                auto &r_bracket(value[2]);
                token_type r = l_bracket;
                if(!arguments.value.empty()){
                    r = join_token(r, arguments);
                }
                r = join_token(r, r_bracket);
                return r;
            }

            token_type make_template_opt(const semantic_type::value_type &value, scanner_type &data){
                return value[0];
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
                    auto iter = pair.begin();
                    const char *other_iter = str;
                    for(; iter != pair.end(); ++iter, ++other_iter){
                        if(*iter != *other_iter){
                            return false;
                        }
                    }
                    if(*other_iter != '\0'){
                        return false;
                    }
                    return true;
                };
                auto &linkdir(value[0]);
                scanner_type::terminal_symbol_linkdir linkdir_value;
                if(linkdir.value.empty()){
                    linkdir_value = scanner_type::nonassoc;
                }else{
                    if(comp(linkdir.value, "left")){
                        linkdir_value = scanner_type::left;
                    }else if(comp(linkdir.value, "right")){
                        linkdir_value = scanner_type::right;
                    }else if(comp(linkdir.value, "nonassoc")){
                        linkdir_value = scanner_type::nonassoc;
                    }else{
                        throw(make_exception(data, "'" + linkdir.value.to_string() + "' 結合方向が次のいずれでもありません, ('left'|'right'|'nonassoc').", linkdir.value));
                    }
                }
                for(
                    auto iter = data.current_terminal_symbol_seq.begin(), end = data.current_terminal_symbol_seq.end();
                    iter != end;
                    ++iter
                ){ (*iter)->linkdir = linkdir_value; }
                data.current_terminal_symbol_seq.clear();
                return eat(value, data);
            }

            token_type make_seq_statements_a(const semantic_type::value_type &value, scanner_type &data){
                auto &identifier(value[0]);
                ++data.current_priority;
                return identifier;
            }

            token_type make_seq_statements_b(const semantic_type::value_type &value, scanner_type &data){
                auto &identifier(value[1]);
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
                auto &identifier(value[0]);
                for(
                    auto iter = data.current_terminal_symbol_seq.begin(), end = data.current_terminal_symbol_seq.end();
                    iter != end;
                    ++iter
                ){ (*iter)->linkdir = scanner_type::nonassoc; }
                data.current_terminal_symbol_seq.clear();
                ++data.current_priority;
                return identifier;
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
                scanner_type::symbol_type symbol = make_symbol(tag, tag.term);
                scanner_type::terminal_symbol_map_type::const_iterator find_ret = data.terminal_symbol_map.find(symbol);
                if(find_ret == data.terminal_symbol_map.end()){
                    throw(make_exception(data, "'" + tag.value.to_string() + "' タグが終端記号ではありません.", tag.value));
                }
                data.current_rhs.tag = tag;
                data.current_rhs.tag.term = find_ret->first.value.term;
                return value[1];
            }

            token_type insert_rhs_element(const token_type &identifier, const token_type &arg, const semantic_type::value_type &value, scanner_type &data){
                scanner_type::symbol_type symbol = make_symbol(identifier, identifier.term);
                scanner_type::terminal_symbol_map_type::const_iterator find_ret_terminal_symbol = data.terminal_symbol_map.find(symbol);
                scanner_type::nonterminal_symbol_map_type::const_iterator
                    find_ret_nonterminal_symbol = data.nonterminal_symbol_map.find(symbol),
                    undefined_nonterminal_symbol_iter;
                bool z = true, w = false;
                if(find_ret_terminal_symbol == data.terminal_symbol_map.end()){
                    z = find_ret_nonterminal_symbol == data.nonterminal_symbol_map.end();
                    if(z){
                        w = true;
                        symbol = make_symbol(identifier, data.next_nonterminal_symbol_id());
                        auto insert_ret = data.nonterminal_symbol_map.insert(std::make_pair(symbol, scanner_type::nonterminal_symbol_data_type()));
                        auto &insert_ret_symbol(insert_ret.first->first);
                        find_ret_nonterminal_symbol = undefined_nonterminal_symbol_iter = insert_ret.first;
                        data.undefined_nonterminal_symbol_set.insert(undefined_nonterminal_symbol_iter->first);
                    }else{
                        symbol = make_symbol(symbol.value, find_ret_nonterminal_symbol->first.value.term);
                    }
                }else{
                    symbol = make_symbol(symbol.value, find_ret_terminal_symbol->first.value.term);
                }
                auto append_arg = [&](const std::string &str_arg_index, const token_type &arg) -> void{
                    int arg_index = std::atoi(str_arg_index.c_str());
                    scanner_type::nonterminal_symbol_data_type::rhs_type &rhs(data.current_rhs);
                    if(rhs.argindex_max < arg_index){ rhs.argindex_max = arg_index; }
                    scanner_type::nonterminal_symbol_data_type::rhs_type::arg_data_type arg_data;
                    arg_data.symbol = symbol;
                    arg_data.number = arg;
                    arg_data.src_index = data.next_rhs_arg_number();
                    if(!rhs.argindex_to_symbol_map.insert(std::make_pair(arg_index, arg_data)).second){
                        throw(make_exception(data, "'" + arg.value.to_string() + "' 引数番号が重複しています.", arg.value));
                    }
                };
                if(!arg.value.empty()){
                    if(find_ret_terminal_symbol != data.terminal_symbol_map.end()){
                        auto &type(find_ret_terminal_symbol->second.type);
                        if(type.value.empty()){
                            throw(make_exception(data, "'" + identifier.value.to_string() + "' 引数となる記号に型が割り当てられていません.", identifier.value));
                        }
                    }
                    std::string arg_str_index(arg.value.begin(), arg.value.end());
                    append_arg(arg_str_index, arg);
                    scanner_type::symbol_type symbol_arg;
                    symbol_arg.value = arg;
                    data.current_rhs.push_back(std::make_pair(symbol, symbol_arg));
                    return join_token(identifier, arg);
                }else{
                    scanner_type::symbol_type dummy_arg;
                    dummy_arg.value = eat(value, data);
                    data.current_rhs.push_back(std::make_pair(symbol, dummy_arg));
                    return identifier;
                }
            }

            token_type make_extended_decl(const semantic_type::value_type &value, scanner_type &data){
                return value[0];
            }

            token_type make_extended_semantic_action(const semantic_type::value_type &value, scanner_type &data){
                return value[0];
            }

            token_type make_extended_decl_opt(const semantic_type::value_type &value, scanner_type &data){
                data.push_extended_data(token_type(), std::vector<token_type>(), value[1], value[6], value[4]);
                return join_token(value.front(), value.back());
            }

            void check_arg(scanner_type &data, scanner_type::nonterminal_symbol_data_type::rhs_type &rhs){
                std::size_t n = rhs.size();
                rhs.argindex_max = n - 1;
                if(n == 1){
                    scanner_type::nonterminal_symbol_data_type::rhs_type::arg_data_type arg_data;
                    arg_data.symbol = make_symbol(rhs[0].first.value, rhs[0].first.value.term);
                    arg_data.src_index = data.next_rhs_arg_number();
                    rhs.argindex_to_symbol_map.insert(std::make_pair(0, arg_data));
                }
            }

            scanner_type::nonterminal_symbol_map_type::iterator make_lhs(
                scanner_type &data,
                const token_type &identifier,
                const token_type &type,
                bool decisied_nonterminal_symbol_term = false,
                term_type nonterminal_symbol_term = epsilon_functor()()
            ){
                scanner_type::nonterminal_symbol_map_type::iterator iter;
                scanner_type::symbol_type undefined_dummy;
                undefined_dummy.value = identifier;
                auto ret = data.undefined_nonterminal_symbol_set.find(undefined_dummy);
                if(ret == data.undefined_nonterminal_symbol_set.end()){
                    scanner_type::symbol_type terminal_symbol = make_symbol(identifier, identifier.term);
                    scanner_type::terminal_symbol_data_type terminal_symbol_data;
                    scanner_type::symbol_type nonterminal_symbol;
                    if(decisied_nonterminal_symbol_term){
                        nonterminal_symbol = make_symbol(identifier, nonterminal_symbol_term);
                    } else{
                        nonterminal_symbol = make_symbol(identifier, data.next_nonterminal_symbol_id());
                    }
                    scanner_type::nonterminal_symbol_data_type nonterminal_symbol_data;
                    nonterminal_symbol_data.type = type;
                    if(data.terminal_symbol_map.find(terminal_symbol) != data.terminal_symbol_map.end()){
                        throw(make_exception(data, "'" + identifier.value.to_string() + "' 終端記号と非終端記号とで衝突しました.", identifier.value));
                    }
                    auto ret = data.nonterminal_symbol_map.insert(std::make_pair(nonterminal_symbol, nonterminal_symbol_data));
                    if(!ret.second){
                        throw(make_exception(data, "'" + identifier.value.to_string() + "' 非終端記号が再宣言されました.", identifier.value));
                    }
                    iter = ret.first;
                } else{
                    auto &undefined_nonterminal_symbol(*ret);
                    scanner_type::symbol_type nonterminal_symbol = make_symbol(identifier, identifier.term);
                    scanner_type::nonterminal_symbol_data_type nonterminal_symbol_data;
                    scanner_type::nonterminal_symbol_map_type::iterator nonterminal_symbol_iter = data.nonterminal_symbol_map.find(nonterminal_symbol);
                    scanner_type::nonterminal_symbol_data_type &deref_nonterminal_symbol(nonterminal_symbol_iter->second);
                    deref_nonterminal_symbol.type = type;
                    data.undefined_nonterminal_symbol_set.erase(ret);
                    iter = nonterminal_symbol_iter;
                }
                return iter;
            }

            void make_ebnf_rule(
                scanner_type &data,
                scanner_type::nonterminal_symbol_data_type &nonterminal_symbol_data,
                const scanner_type::extended_data_type &extended_data
            ){
                const token_type &token(extended_data.identifier);
                if(data.extended_rule_is_scanned(token)){ return; }
                auto erase_undefined_info = [&](const token_type &t){
                    auto iter = data.undefined_nonterminal_symbol_set.find(scanner_type::symbol_type(t));
                    if(iter != data.undefined_nonterminal_symbol_set.end()){
                        data.undefined_nonterminal_symbol_set.erase(iter);
                    }
                };
                auto push_back_token_seq = [&](scanner_type::nonterminal_symbol_data_type::rhs_type &rhs, const std::vector<token_type> &seq){
                    for(const auto &x : seq){
                        rhs.push_back(std::make_pair(scanner_type::symbol_type(x), scanner_type::symbol_type()));
                    }
                };
                if(extended_data.decl.value.to_string() == "?"){
                    scanner_type::nonterminal_symbol_data_type::rhs_type rhs;
                    push_back_token_seq(rhs, extended_data.token_seq);
                    rhs.semantic_action = extended_data.semantic_action;
                    nonterminal_symbol_data.rhs.insert(rhs);
                    nonterminal_symbol_data.rhs.insert(scanner_type::nonterminal_symbol_data_type::rhs_type());
                    insert_rhs(data, rhs, nonterminal_symbol_data, true);
                    erase_undefined_info(extended_data.identifier);
                } else if(extended_data.decl.value.to_string() == "*"){
                    scanner_type::nonterminal_symbol_data_type::rhs_type rhs_a, rhs_b;
                    rhs_a.push_back(std::make_pair(scanner_type::symbol_type(extended_data.identifier), scanner_type::symbol_type()));
                    push_back_token_seq(rhs_a, extended_data.token_seq);
                    rhs_a.semantic_action = extended_data.semantic_action;
                    insert_rhs(data, rhs_a, nonterminal_symbol_data, true);
                    push_back_token_seq(rhs_b, extended_data.token_seq);
                    rhs_b.semantic_action = extended_data.semantic_action;
                    insert_rhs(data, rhs_b, nonterminal_symbol_data, true);
                    nonterminal_symbol_data.rhs.insert(scanner_type::nonterminal_symbol_data_type::rhs_type());
                    erase_undefined_info(extended_data.identifier);
                } else if(extended_data.decl.value.to_string() == "+"){
                    scanner_type::nonterminal_symbol_data_type::rhs_type rhs_a, rhs_b;
                    rhs_a.push_back(std::make_pair(scanner_type::symbol_type(extended_data.identifier), scanner_type::symbol_type()));
                    push_back_token_seq(rhs_a, extended_data.token_seq);
                    rhs_a.semantic_action = extended_data.semantic_action;
                    insert_rhs(data, rhs_a, nonterminal_symbol_data, true);
                    push_back_token_seq(rhs_b, extended_data.token_seq);
                    rhs_b.semantic_action = extended_data.semantic_action;
                    insert_rhs(data, rhs_b, nonterminal_symbol_data, true);
                    erase_undefined_info(extended_data.identifier);
                }
                return;
            }

            token_type make_rhs_seq_element(const semantic_type::value_type &value, scanner_type &data){
                if(value[1].value.empty()){
                    return value[0];
                }else{
                    token_type t = join_token(value[0], value[1]);
                    data.current_extended_data().token_seq.push_back(value[0]);
                    data.current_extended_data().identifier = t;
                    return t;
                }
            }

            token_type make_rhs_seq_group(const semantic_type::value_type &value, scanner_type &data){
                if(value[3].value.empty()){
                    return value[1];
                }else{
                    token_type t = join_token(value[0], value[2]);
                    data.current_extended_data().token_seq.push_back(value[1]);
                    data.current_extended_data().identifier = t;
                    return t;
                }
            }

            token_type make_rhs_union(const semantic_type::value_type &value, scanner_type &data){
                if(!data.ref_current_group_seq.seq.empty()){
                    data.register_group_seq();
                }
                return value[0];
            }

            token_type make_rhs_union_rest(const semantic_type::value_type &value, scanner_type &data){
                return value[2];
            }

            token_type make_rhs_seq(const semantic_type::value_type &value, scanner_type &data){
                token_type r = insert_rhs_element(value[0], value[1], value, data);
                data.inc_current_rhs_arg_number();
                if(data.ref_current_group_seq.head_is_scanned){
                    data.register_group_seq();
                }
                data.push_group_seq_element(value[0], value[1]);
                return r;
            }

            token_type make_rhs_seq_last(const semantic_type::value_type &value, scanner_type &data){
                token_type r = insert_rhs_element(value[1], value[2], value, data);
                data.inc_current_rhs_arg_number();
                data.push_group_seq_element(value[1], value[2]);
                return r;
            }

            token_type make_rhs_seq_opt(const semantic_type::value_type &value, scanner_type &data){
                data.dispose_group_seq();
                return value[0];
            }

            const scanner_type::nonterminal_symbol_data_type::rhs_type &insert_rhs(scanner_type &data){
                return insert_rhs(data, data.current_rhs, data.current_nonterminal_symbol_iter->second);
            }

            void construct_lambda(scanner_type &data){
                for(
                    scanner_type::extended_data_type *ptr = &data.current_extended_data();
                    !data.current_extended_data_is_empty();
                    data.pop_current_extended_data()
                ){
                    scanner_type::extended_data_type &extended_data(data.current_extended_data());
                    auto f = [&](token_type &token){
                        std::string debug_string(token.value.to_string());
                        scanner_type::symbol_type terminal_dummy(token);
                        auto terminal_iter = data.terminal_symbol_map.find(terminal_dummy);
                        if(terminal_iter != data.terminal_symbol_map.end()){
                            token.term = terminal_iter->first.value.term;
                        } else{
                            auto nonterminal_iter = data.nonterminal_symbol_map.find(scanner_type::symbol_type(token));
                            token.term = nonterminal_iter->first.value.term;
                        }
                    };
                    f(extended_data.identifier);
                    for(auto &x : extended_data.token_seq){ f(x); }
                    scanner_type::nonterminal_symbol_data_type &nonterminal_symbol_data(make_lhs(data, extended_data.identifier, extended_data.type)->second);
                    make_ebnf_rule(
                        data,
                        nonterminal_symbol_data,
                        extended_data
                    );
                    data.current_rhs.clear();
                }
            }

            token_type make_rhs(const semantic_type::value_type &value, scanner_type &data){
                auto &rhs(insert_rhs(data));
                for(int i = 0, n = rhs.argindex_max; i <= n; ++i){
                    if(rhs.argindex_to_symbol_map.find(i) == rhs.argindex_to_symbol_map.end()){
                        scanner_type::nonterminal_symbol_data_type::rhs_type::argindex_to_symbol_map_type::const_iterator iter;
                        int j = i + 1;
                        for(; j < n; ++j){
                            iter = rhs.argindex_to_symbol_map.find(j);
                            if(iter == rhs.argindex_to_symbol_map.end()){
                                continue;
                            }
                        }
                        if(j == n){
                            iter = rhs.argindex_to_symbol_map.find(n);
                        }
                        throw(make_exception(data, "引数が連続していません.", iter->second.number.value));
                    }
                }
                data.current_rhs.clear();
                data.clear_current_rhs_arg_number();
                construct_lambda(data);
                data.clear_extended_data();
                return eat(value, data);
            }

            token_type make_rhs_rest(const semantic_type::value_type &value, scanner_type &data){
                data.clear_rhs_number();
                return eat(value, data);
            }

            token_type make_lhs(const semantic_type::value_type &value, scanner_type &data){
                auto &identifier(value[0]);
                auto &type(value[1]);
                bool decisied_nonterminal_symbol_term = false;
                term_type nonterminal_symbol_term = epsilon_functor()();
                if(!data.get_scanned_first_nonterminal_symbol()){
                    data.first_nonterminal_symbol.value = identifier;
                    nonterminal_symbol_term = data.next_nonterminal_symbol_id();
                    data.first_nonterminal_symbol.value.term = nonterminal_symbol_term;
                    data.set_scanned_first_nonterminal_symbol();
                    decisied_nonterminal_symbol_term = true;
                }
                data.current_nonterminal_symbol_iter = make_lhs(
                    data,
                    data.first_nonterminal_symbol.value,
                    type,
                    decisied_nonterminal_symbol_term, nonterminal_symbol_term
                );
                return join_token(identifier, type);
            }

            token_type make_lhs_type(const semantic_type::value_type &value, scanner_type &data){
                return value[1];
            }

            token_type make_expression(const semantic_type::value_type &value, scanner_type &data){
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
                auto str(header.value.to_string());
                if(data.analysis_phase == scanner_type::phase_token){
                    if(str == "token_asc"){
                        data.token_order = scanner_type::order_ascending;
                    }else if(str == "token_desc"){
                        data.token_order = scanner_type::order_descending;
                    }else{
                        throw(make_exception(data, "トークンヘッダが次のいずれでもありません, ('token_asc'|'token_desc').", header.value));
                    }
                }else if(data.analysis_phase == scanner_type::phase_grammar && str != "lalr"){
                    throw(make_exception(data, "構文解析ヘッダが次のいずれでもありません, ('lalr')", header.value));
                }
                return header;
            }

            token_type make_token_header_rest(const semantic_type::value_type &value, scanner_type &data){
                auto &identifier(value[1]);
                if(identifier.value.to_string() != "extern"){
                    throw(make_exception(data, "拡張トークンヘッダが次のいずれでもありません, ('extern')", identifier.value));
                }
                data.external_token = true;
                return identifier;
            }

            token_type make_token_body(const semantic_type::value_type &value, scanner_type &data){
                data.analysis_phase = scanner_type::phase_grammar;
                return eat(value, data);
            }

            token_type make_grammar_body(const semantic_type::value_type &value, scanner_type &data){
                data.analysis_phase = scanner_type::phase_end;
                return eat(value, data);
            }

            token_type make_default_semantic_action(const semantic_type::value_type &value, scanner_type &data){
                data.default_semantic_action = value[1].value;
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

        namespace{
            scanner_type::expression_type start_prime;
        }

        void define_scanner_grammar(scanner_type &scanner){
            term_type current_nonterminal_symbol_id = 1;
#define DECL(name) \
    scanner_type::expression_type name; \
    ([&]() -> void{ \
        name.lhs = -(current_nonterminal_symbol_id++); \
        term_to_str_map[name.lhs] = BOOST_PP_STRINGIZE(name); \
    })();
            DECL(StartPrime);
            DECL(Start);
            DECL(IdentifierSeq);
            DECL(NonDelimIdentifierSeq);
            DECL(Type);
            DECL(ReferenceSpecifier);
            DECL(ReferenceSpecifier_opt);
            DECL(DoubleColon_opt);
            DECL(NestIdentifier);
            DECL(NestIdentifier_opt);
            DECL(Template);
            DECL(Template_opt);
            DECL(TemplateArg_opt);
            DECL(TypeSeq);
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
            DECL(ExtendedDecl);
            DECL(ExtendedSemanticAction_opt);
            DECL(ExtendedDecl_opt);
            DECL(RHSSeqElement);
            DECL(RHSSeqUnion);
            DECL(RHSSeq);
            DECL(RHSSeq_opt);
            DECL(RHS);
            DECL(LHS);
            DECL(Exp);
            DECL(ExpStatements);
            DECL(ExpStatementsRest);
            DECL(DefaultSemanticAction_opt);
            DECL(TokenHeader);
            DECL(TokenHeaderRest_opt);
            DECL(GrammarHeader);
            DECL(GrammarNamespace);
            DECL(TokenNamespace);
            DECL(TokenBody);
            DECL(GrammarBody);
#undef DECL

            using namespace terminal_symbol;
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
                NonDelimIdentifierSeq,
                ((identifier)(ReferenceSpecifier_opt.lhs))                          (make_non_delim_identifier_seq_a)
                ((NonDelimIdentifierSeq.lhs)(identifier)(ReferenceSpecifier_opt.lhs))
                (make_non_delim_identifier_seq_b)
                ((ReferenceSpecifier.lhs))                                          (make_non_delim_identifier_seq_c)
            );

            DECL_SEQS(
                Type,
                ((DoubleColon_opt.lhs)(NonDelimIdentifierSeq.lhs)(Template_opt.lhs)(NestIdentifier_opt.lhs))
                (make_type_a)
                ((Type.lhs)(NonDelimIdentifierSeq.lhs))                             (make_type_b)
            );

            DECL_SEQS(
                ReferenceSpecifier,
                ((asterisk))                                                        (make_reference_specifier)
                ((ampersand))                                                       (make_reference_specifier)
            );

            DECL_SEQS_EPS(
                ReferenceSpecifier_opt,
                ((ReferenceSpecifier.lhs))                                          (make_reference_specifier_opt)
            );

            DECL_SEQS_EPS(
                DoubleColon_opt,
                ((double_colon))                                                    (make_double_colon_opt)
            );

            DECL_SEQS_EPS(
                NestIdentifier_opt,
                ((NestIdentifier.lhs))                                              (make_nest_identifier_opt)
            );

            DECL_SEQS(
                NestIdentifier,
                ((double_colon)(identifier)(Template_opt.lhs))                      (make_nest_identifier_a)
                ((dot)(identifier)(Template_opt.lhs))                               (make_nest_identifier_a)
                ((NestIdentifier.lhs)(double_colon)(identifier)(Template_opt.lhs))  (make_nest_identifier_b)
                ((NestIdentifier.lhs)(dot)(identifier)(Template_opt.lhs))           (make_nest_identifier_b)
            );

            DECL_SEQS(
                Template,
                ((l_bracket)(TemplateArg_opt.lhs)(r_bracket))                       (make_template)
            );

            DECL_SEQS_EPS(
                Template_opt,
                ((Template.lhs))                                                    (make_template_opt)
            );

            DECL_SEQS_EPS(
                TemplateArg_opt,
                ((TypeSeq.lhs))                                                     (make_template_arg)
            );

            DECL_SEQS(
                TypeSeq,
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
                ((l_bracket)(value)(r_bracket))                                     (make_arg)
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
                ExtendedDecl,
                ((question))                                                        (make_extended_decl)
                ((asterisk))                                                        (make_extended_decl)
                ((plus))                                                            (make_extended_decl)
            );

            DECL_SEQS_EPS(
                ExtendedSemanticAction_opt,
                ((identifier))                                                      (make_extended_semantic_action)
            );

            DECL_SEQS_EPS(
                ExtendedDecl_opt,
                ((l_square_bracket)(ExtendedSemanticAction_opt.lhs)(r_square_bracket)(l_bracket)(identifier)(r_bracket)(ExtendedDecl.lhs))
                                                                                    (make_extended_decl_opt)
            );

            DECL_SEQS(
                RHSSeqElement,
                ((identifier)(ExtendedDecl_opt.lhs))                                (make_rhs_seq_element)
                ((l_round_pare)(RHSSeqUnion.lhs)(r_round_pare)(ExtendedDecl_opt.lhs))
                                                                                    (make_rhs_seq_group)
            );

            DECL_SEQS(
                RHSSeqUnion,
                ((RHSSeq.lhs))                                                      (make_rhs_union)
                // 未調整
                ((RHSSeqUnion.lhs)(symbol_or)(RHSSeq.lhs))                          (make_rhs_union_rest)
            );

            DECL_SEQS(
                RHSSeq,
                ((RHSSeqElement.lhs)(Arg_opt.lhs))                                  (make_rhs_seq)
                ((RHSSeq.lhs)(RHSSeqElement.lhs)(Arg_opt.lhs))                      (make_rhs_seq_last)
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
                ((LHS.lhs)(symbol_colon)(RHS.lhs))                                  (make_expression)
            );

            DECL_SEQS(
                ExpStatements,
                ((Exp.lhs)(semicolon))                                              (make_exp_statements_a)
                ((semicolon))                                                       (eat)
                ((ExpStatements.lhs)(Exp.lhs)(semicolon))                           (make_exp_statements_b)
                ((ExpStatements.lhs)(semicolon))                                    (eat)
            );

            DECL_SEQS_EPS(
                TokenHeaderRest_opt,
                ((comma)(identifier))                                               (make_token_header_rest)
            );

            DECL_SEQS(
                TokenHeader,
                ((l_bracket)(identifier)(TokenHeaderRest_opt.lhs)(r_bracket))       (make_header)
            );

            DECL_SEQS(
                TokenBody,
                ((l_curly_bracket)(TopLevelSeqStatements.lhs)(r_curly_bracket))     (make_token_body)
            );

            DECL_SEQS(
                GrammarHeader,
                ((l_bracket)(identifier)(r_bracket))                                (make_header)
            );

            DECL_SEQS_EPS(
                DefaultSemanticAction_opt,
                ((l_square_bracket)(identifier)(r_square_bracket))                 (make_default_semantic_action)
            );

            DECL_SEQS(
                GrammarBody,
                ((l_curly_bracket)(DefaultSemanticAction_opt.lhs)(ExpStatements.lhs)(r_curly_bracket))
                (make_grammar_body)
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
                ((TokenHeader.lhs)(TokenNamespace.lhs)(TokenBody.lhs)(GrammarHeader.lhs)(GrammarNamespace.lhs)(GrammarBody.lhs))
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

            start_prime = StartPrime;
        }

        void scanner_type::define_grammar(scanner_type &scanner){
            static bool flag = true;
            if(flag){
                for(
                    auto iter = terminal_symbol::aux::terminal_symbol_data.begin(), end = terminal_symbol::aux::terminal_symbol_data.end();
                    iter != end;
                    ++iter
                ){
                    scanner.add_terminal_symbol(*iter, symbol_data_type());
                }
                terminal_symbol::aux::terminal_symbol_data.clear();
                define_scanner_grammar(scanner);
                flag = false;
            }
        }

        template<class PutFn, class ErrorFn>
        bool scanner_type::parse(const PutFn &put_fn, const ErrorFn &error_fn){
            typedef std::vector<term_type>      term_stack_type;
            typedef std::vector<std::size_t>    state_stack_type;
            typedef std::vector<token_type>     value_stack_type;
            term_stack_type     term_stack;
            state_stack_type    state_stack;
            value_stack_type    value_stack;
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
                if(action.action_kind == action_shift){
                    term_stack.push_back(a);
                    state_stack.push_back(action.action_number);
                    value_stack.push_back(t);
                    ++iter;
                }else if(action.action_kind == action_reduce){
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
                }else if(action.action_kind == action_acc){
                    break;
                }
            }
            return true;
        }

        void scanner_type::scan(std::istream &in){
            define_grammar(*this);
            {
                std::string str_;
                std::getline(in, str_, '\0');
                string.reserve(str_.size());
                string.assign(str_.begin(), str_.end());
            }
            scanner_string_type::iterator begin = string.begin(), end = string.end();
            lexer::tokenize(
                begin,
                end,
                [&](
                    scanner_type::term_type t,
                    scanner::iterator<scanner_string_type::iterator> a,
                    scanner::iterator<scanner_string_type::iterator> b
                ){
                    token_seq.push_back(
                        token_type(
                            string_iter_pair_type(a.place, b.place),
                            t, b.char_count, b.line_count
                        )
                    );
                }
            );
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

            make_parsing_table_options_type options;
            options.avoid_conflict = true;
            options.disambiguating = true;
            // options.put_log = true; // デバッグ用

            bool result = make_parsing_table(
                start_prime,
                dummy_term,
                options,
                is_not_terminal,
                term_to_str
            );

            if(!result){ // !! ブート用の文法が間違っている
                return;
            }

            {
                /*
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
                */

                //ログを残さない
                auto put_fn = [](const item_type&) -> void{  };
                auto error_fn = [&](const token_seq_type::const_iterator &iter) -> void{
                    throw(make_exception(*this, "シンタックスエラー.", iter->value));
                };

                parse(put_fn, error_fn);
            }

            check_undefined_nonterminal_symbol();
            check_linked_nonterminal_symbol();
            normalize_token_order();
            collect_token();
            caching_arg_type();
            augment();
        }

        namespace terminal_symbol{
#define DECL(name) scanner_type::term_type name;
            KP19PP_SCANNER_DECL_TERMINAL_SYMBOLS();
#undef DECL
        }
    }
}

