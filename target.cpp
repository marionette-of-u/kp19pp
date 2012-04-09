#include "target.hpp"

namespace kp19pp{
    namespace target{
        term_type epsilon_functor::operator ()() const{
            return 0;
        }

        term_type end_of_seq_functor::operator ()() const{
            return (std::numeric_limits<int>::max)();
        }

        target_type::target_type() : base_type(){}
        target_type::target_type(const target_type &other) : base_type(other){}
        target_type::target_type(target_type &&other) : base_type(other){}

        bool target_type::make_parsing_table(
            const scanner::scanner_type &scanner,
            const commandline_options_type &commandline_options
        ){
            expression_type expression_start_prime;
            expression_start_prime.lhs = -1;
            {
                expression_type::rhs_type rhs_seq;
                rhs_seq.push_back(-2);
                expression_start_prime.rhs.insert(rhs_seq);
            }
            add_expression(expression_start_prime);
            for(
                auto iter = scanner.nonterminal_symbol_map.begin(), end = scanner.nonterminal_symbol_map.end();
                iter != end;
                ++iter
            ){
                auto &lhs(iter->first);
                auto &data(iter->second);
                expression_type e;
                e.lhs = lhs.value.term;
                for(
                    auto rhs_set_iter = data.rhs.begin(), rhs_set_end = data.rhs.end();
                    rhs_set_iter != rhs_set_end;
                    ++rhs_set_iter
                ){
                    auto &rhs(*rhs_set_iter);
                    expression_type::rhs_type rhs_seq;
                    rhs_seq.resize(rhs.size());
                    std::size_t i = 0;
                    for(auto rhs_iter = rhs.begin(), rhs_end = rhs.end(); rhs_iter != rhs_end; ++rhs_iter, ++i){
                        auto &rhs_elem(*rhs_iter);
                        rhs_seq[i] = rhs_elem.first.value.term;
                    }
                    if(!rhs.tag.value.is_epsilon()){
                        rhs_seq.set_attribute(0, rhs.tag.term);
                    }
                    rhs_seq.semantic.type = &data.type.value;
                    rhs_seq.semantic.action = &rhs.semantic_action.value;
                    rhs_seq.semantic.argindex_to_symbol_map = &rhs.argindex_to_symbol_map;
                    e.rhs.insert(rhs_seq);
                }
                add_expression(e);
            }
            add_terminal_symbol(epsilon_functor()(), symbol_data_type());
            add_terminal_symbol(end_of_seq_functor()(), symbol_data_type());
            for(
                auto iter = scanner.terminal_symbol_map.begin(), end = scanner.terminal_symbol_map.end();
                iter != end;
                ++iter
            ){
                auto &data(iter->second);
                symbol_data_type symbol_data;
                symbol_data.linkdir = data.linkdir;
                symbol_data.priority = data.priority;
                add_terminal_symbol(iter->first.value.term, symbol_data);
            }
            make_parsing_table_options_type options;
            options.avoid_conflict = true;
            bool result = base_type::make_parsing_table(
                expression_start_prime,
                (std::numeric_limits<term_type>::max)() - 1,
                options,
                [](const expression_set_type&, term_type term) -> bool{ return term < 0; },
                [&](term_type term) -> std::string{
                    if(term == expression_start_prime.lhs){
                        return "S'";
                    }else if(term == end_of_seq_functor()()){
                        return "$";
                    }else{
                        auto find_ret = scanner.term_to_token_map.find(term);
                        return std::string(
                            find_ret->second.value.begin(),
                            find_ret->second.value.end()
                        );
                    }
                }
            );
            if(!result){
                return false;
            }
            // TODO : generating parser
            return true;
        }
    }
}

