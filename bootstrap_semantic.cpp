#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include "scanner_lexer.hpp"
#include "scanner.hpp"
#include "exception.hpp"

namespace kp19pp{
    namespace scanner{
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

            semantic_type::token_type eat(const semantic_type::token_type &arg_0){

            }

            semantic_type::token_type identifier_seq_a(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1){

            }

            semantic_type::token_type identifier_seq_b(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1, const semantic_type::token_type &arg_2, const semantic_type::token_type &arg_3){

            }

            semantic_type::token_type make_arg(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1, const semantic_type::token_type &arg_2){

            }

            semantic_type::token_type make_block_with_linkdir(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1, const semantic_type::token_type &arg_2, const semantic_type::token_type &arg_3){

            }

            semantic_type::token_type make_default_semantic_action(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1, const semantic_type::token_type &arg_2){

            }

            semantic_type::token_type make_double_colon_opt(const semantic_type::token_type &arg_0){

            }

            semantic_type::token_type make_exp_statements_a(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1){

            }

            semantic_type::token_type make_exp_statements_b(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1, const semantic_type::token_type &arg_2){

            }

            semantic_type::token_type make_expression(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1, const semantic_type::token_type &arg_2){

            }

            semantic_type::token_type make_grammar_body(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1, const semantic_type::token_type &arg_2, const semantic_type::token_type &arg_3){

            }

            semantic_type::token_type make_grammar_namespace(const semantic_type::token_type &arg_0){

            }

            semantic_type::token_type make_header(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1, const semantic_type::token_type &arg_2, const semantic_type::token_type &arg_3){

            }

            semantic_type::token_type make_lhs(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1){

            }

            semantic_type::token_type make_lhs_type(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1, const semantic_type::token_type &arg_2){

            }

            semantic_type::token_type make_link_dir(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1, const semantic_type::token_type &arg_2){

            }

            semantic_type::token_type make_nest_identifier_a(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1, const semantic_type::token_type &arg_2){

            }

            semantic_type::token_type make_nest_identifier_b(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1, const semantic_type::token_type &arg_2, const semantic_type::token_type &arg_3){

            }

            semantic_type::token_type make_nest_identifier_opt(const semantic_type::token_type &arg_0){

            }

            semantic_type::token_type make_non_delim_identifier_seq_a(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1){

            }

            semantic_type::token_type make_non_delim_identifier_seq_b(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1, const semantic_type::token_type &arg_2){

            }

            semantic_type::token_type make_non_delim_identifier_seq_c(const semantic_type::token_type &arg_0){

            }

            semantic_type::token_type make_reference_specifier(const semantic_type::token_type &arg_0){

            }

            semantic_type::token_type make_reference_specifier_opt(const semantic_type::token_type &arg_0){

            }

            semantic_type::token_type make_rhs(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1, const semantic_type::token_type &arg_2){

            }

            semantic_type::token_type make_rhs_seq(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1){

            }

            semantic_type::token_type make_rhs_seq_last(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1, const semantic_type::token_type &arg_2){

            }

            semantic_type::token_type make_rhs_seq_opt(const semantic_type::token_type &arg_0){

            }

            semantic_type::token_type make_semantic_action(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1, const semantic_type::token_type &arg_2){

            }

            semantic_type::token_type make_semantic_action_opt(const semantic_type::token_type &arg_0){

            }

            semantic_type::token_type make_seq_statements_a(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1){

            }

            semantic_type::token_type make_seq_statements_b(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1, const semantic_type::token_type &arg_2){

            }

            semantic_type::token_type make_seq_statements_element(const semantic_type::token_type &arg_0){

            }

            semantic_type::token_type make_symbol_type(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1, const semantic_type::token_type &arg_2){

            }

            semantic_type::token_type make_tag(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1, const semantic_type::token_type &arg_2){

            }

            semantic_type::token_type make_template(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1, const semantic_type::token_type &arg_2){

            }

            semantic_type::token_type make_template_arg(const semantic_type::token_type &arg_0){

            }

            semantic_type::token_type make_template_opt(const semantic_type::token_type &arg_0){

            }

            semantic_type::token_type make_token_body(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1, const semantic_type::token_type &arg_2){

            }

            semantic_type::token_type make_token_header_rest(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1){

            }

            semantic_type::token_type make_token_namespace(const semantic_type::token_type &arg_0){

            }

            semantic_type::token_type make_top_level_seq_statements_a(const semantic_type::token_type &arg_0){

            }

            semantic_type::token_type make_top_level_seq_statements_b(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1){

            }

            semantic_type::token_type make_top_level_seq_statements_element_a(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1){

            }

            semantic_type::token_type make_top_level_seq_statements_element_b(const semantic_type::token_type &arg_0){

            }

            semantic_type::token_type make_type_a(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1, const semantic_type::token_type &arg_2, const semantic_type::token_type &arg_3){

            }

            semantic_type::token_type make_type_b(const semantic_type::token_type &arg_0, const semantic_type::token_type &arg_1){

            }

            semantic_type::token_type make_type_seq(const semantic_type::token_type &arg_0){

            }

        }
    }
}
