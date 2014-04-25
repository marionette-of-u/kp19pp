#include <list>
#include <sstream>
#include <cstdio>
#include <cctype>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include "target.hpp"
#include "common.hpp"
#include "indent.hpp"

namespace kp19pp{
    namespace target{
        typedef scanner::scanner_type::nonterminal_symbol_data_type::rhs_type::argindex_to_symbol_map_type argindex_to_symbol_map_type;

        std::vector<string_iter_pair_type> make_signature(
            const string_iter_pair_type &semantic_action,
            const string_iter_pair_type &return_type,
            const argindex_to_symbol_map_type &argindex_to_symbol_map
        );

        void target_type::generate_vimscript(
            std::ostream &os,
            const commandline_options_type &commandline_options,
            const scanner::scanner_type &scanner
        ){
            indent_type indent_1(commandline_options.indent(), 1);

            // name prefix
            auto s_prefix = [](const std::string &str, int underscore){ return "let s:" + str + (underscore ? "_" : ""); };
            auto s_function = [](const std::string &str, const std::string &args){ return "function! s:" + str + "(" + args + ")" + "\n"; };
            const std::string endfunction = "endfunction\n";
            auto while_ = [](const std::string &cond){ return "while " + cond + "\n"; };
            const std::string endwhile = "endwhile\n";

            // token enumeration
            {
                std::size_t i = 0;
                os << s_prefix(scanner.namespace_token.empty() ? "token" : scanner.namespace_token.to_string(), 0) << " = {";
                for(; i < scanner.number_to_token_map.size(); ++i){
                    auto find_ret(scanner.number_to_token_map.find(i)->second);
                    if(i != 0){ os << ","; }
                    os << " '" << find_ret.value << "': " << i;
                }
                if(i != 0){ os << ","; }
                os << " 'token_0': " << i;
            }
            os << " }\n";
            os << "\n";

            // stack
            os << s_prefix("stack", 0) << " = {}\n";
            // ctor
            os << s_function("stack.ctor", "");
            os << indent_1 << "let self.gap_ = 0" << "\n";
            os << indent_1 << "let self.tmp_ = []" << "\n";
            os << indent_1 << "let self.stack_ = []" << "\n";
            os << endfunction;
            os << "\n";
            // reset_tmp
            os << s_function("stack.reset_tmp", "");
            os << indent_1 << "let self.gap_ = len(self.stack_)\n";
            os << indent_1 << "let self.tmp_ = []\n";
            os << endfunction;
            os << "\n";
            // commit_tmp
            os << s_function("stack.commit_tmp", "");
            os << indent_1 << "if len(self.stack_) > self.gap_\n";
            os << indent_1 << indent_1 << "call remove(self.stack_, self.gap_, len(self.stack_) - 1)\n";
            os << indent_1 << "endif\n";
            os << indent_1 << "let l:i = 0\n";
            os << indent_1 << "while l:i < len(self.tmp_)\n";
            os << indent_1 << indent_1 << "call add(self.stack_, self.tmp_[i])\n";
            os << indent_1 << indent_1 << "let l:i += 1\n";
            os << indent_1 << "endwhile\n";
            os << endfunction;
            os << "\n";
            // push
            os << s_function("stack.push", "f");
            os << indent_1 << "call add(self.tmp_, copy(a:f))\n";
            os << endfunction;
            os << "\n";
            // pop
            os << s_function("stack.pop", "n_");
            os << indent_1 << "let l:n = a:n_\n";
            os << indent_1 << "if len(self.tmp_) < l:n\n";
            os << indent_1 << indent_1 << "let l:n -= len(self.tmp_)\n";
            os << indent_1 << indent_1 << "let self.tmp_ = []\n";
            os << indent_1 << indent_1 << "let self.gap_ -= l:n\n";
            os << indent_1 << "else\n";
            os << indent_1 << indent_1 << "call remove(self.tmp_, len(self.tmp_) - l:n, len(self.tmp_) - 1)\n";
            os << indent_1 << "endif\n";
            os << endfunction;
            os << "\n";
            // top
            os << s_function("stack.top", "");
            os << indent_1 << "if len(self.tmp_) > 0\n";
            os << indent_1 << indent_1 << "return self.tmp_[len(self.tmp_) - 1]\n";
            os << indent_1 << "else\n";
            os << indent_1 << indent_1 << "return self.stack_[self.gap_ - 1]\n";
            os << indent_1 << "endif\n";
            os << endfunction;
            os << "\n";
            // get_arg
            os << s_function("stack.get_arg", "base, index");
            os << indent_1 << "let l:n = len(self.tmp_)\n";
            os << indent_1 << "if (a:base - a:index) <= l:n\n";
            os << indent_1 << indent_1 << "return self.tmp_[l:n - (a:base - a:index)]\n";
            os << indent_1 << "else\n";
            os << indent_1 << indent_1 << "return self.stack_[self.gap_ - (a:base - l:n) + a:index]\n";
            os << indent_1 << "endif\n";
            os << endfunction;
            os << "\n";
            // clear
            os << s_function("stack.clear", "");
            os << indent_1 << "let self.stack_ = []\n";
            os << endfunction;
            os << "\n";

            // stack frame
            os << s_prefix("stack_frame", 0) << " = {}\n";
            // ctor
            os << s_function("stack_frame.ctor", "s, g, v");
            os << indent_1 << "let self.state = copy(a:s)\n";
            os << indent_1 << "let self.gotof = copy(a:g)\n";
            os << indent_1 << "let self.value = copy(a:v)\n";
            os << endfunction;
            os << "\n";

            // parser
            os << s_prefix("parser", 0) << " = {}\n";
            // ctor
            os << s_function("parser.ctor", "sa");
            os << indent_1 << "let self.sa_ = copy(a:sa)\n";
            os << indent_1 << "let self.stack_ = copy(s:stack)\n";
            os << indent_1 << "call self.stack_.ctor()\n";
            os << indent_1 << "call self.reset()\n";
            os << endfunction;
            os << "\n";
            // reset
            os << s_function("parser.reset", "");
            os << indent_1 << "let self.error_ = 0\n";
            os << indent_1 << "let self.accepted_ = 0\n";
            os << indent_1 << "call self.clear_stack()\n";
            os << indent_1 << "call self.reset_tmp_stack()\n";
            os << indent_1 << "call self.push_stack('state_0', 'gotof_0', 0)\n";
            os << indent_1 << "call self.commit_tmp_stack()\n";
            os << endfunction;
            os << "\n";
            // post
            os << s_function("parser.post", "token, value");
            os << indent_1 << "call self.reset_tmp_stack()\n";
            os << indent_1 << "while self[self.stack_top().state](a:token, a:value)\n";
            os << indent_1 << "endwhile\n";
            os << indent_1 << "if self.error_ == 0\n";
            os << indent_1 << indent_1 << "call self.commit_tmp_stack()\n";
            os << indent_1 << "endif\n";
            os << indent_1 << "return self.accepted_ || self.error_\n";
            os << endfunction;
            os << "\n";
            // accept
            os << s_function("parser.accept", "");
            os << indent_1 << "if self.error_ == 1\n";
            os << indent_1 << indent_1 << "return 0\n";
            os << indent_1 << "endif\n";
            os << indent_1 << "return 1\n";
            os << endfunction;
            os << "\n";
            // clear_stack
            os << s_function("parser.clear_stack", "");
            os << indent_1 << "call self.stack_.clear()\n";
            os << endfunction;
            os << "\n";
            // reset_tmp_stack
            os << s_function("parser.reset_tmp_stack", "");
            os << indent_1 << "call self.stack_.reset_tmp()\n";
            os << endfunction;
            os << "\n";
            // push_stack
            os << s_function("parser.push_stack", "s, g, v");
            os << indent_1 << "let l:x = copy(s:stack_frame)\n";
            os << indent_1 << "call l:x.ctor(a:s, a:g, a:v)\n";
            os << indent_1 << "call self.stack_.push(l:x)\n";
            os << indent_1 << "return 1\n";
            os << endfunction;
            os << "\n";
            // pop_stack
            os << s_function("parser.pop_stack", "n");
            os << indent_1 << "call self.stack_.pop(a:n)\n";
            os << endfunction;
            os << "\n";
            // commit_tmp_stack
            os << s_function("parser.commit_tmp_stack", "");
            os << indent_1 << "call self.stack_.commit_tmp()\n";
            os << endfunction;
            os << "\n";
            // stack_top
            os << s_function("parser.stack_top", "");
            os << indent_1 << "return self.stack_.top()\n";
            os << endfunction;
            os << "\n";
            // get_arg
            os << s_function("parser.get_arg", "base, index");
            os << indent_1 << "return self.stack_.get_arg(a:base, a:index)['value']\n";
            os << endfunction;
            os << "\n";

            // member function signature -> index
            std::map<std::vector<string_iter_pair_type>, std::size_t> stub_index;
            {
                // member function name -> count
                std::map<string_iter_pair_type, std::size_t> stub_count;

                // action handler stub
                for(
                    auto table_iter = ref_parsing_table.begin(), table_end = ref_parsing_table.end();
                    table_iter != table_end;
                    ++table_iter
                ){
                    auto &actions(*table_iter->actions);
                    for(
                        auto action_iter = actions.begin(), action_end = actions.end();
                        action_iter != action_end;
                        ++action_iter
                    ){
                        auto &action(action_iter->second);
                        if(action.action_kind != action_reduce){ continue; }
                        auto &semantic(action.item->rhs->semantic);

                        // make signature
                        std::vector<string_iter_pair_type> signature;

                        // ... function name
                        signature.push_back(*semantic.action);

                        // ... return value
                        signature.push_back(*semantic.type);

                        // ... arguments
                        for(
                            std::size_t i = 0, i_length = semantic.argindex_to_symbol_map->size();
                            i < i_length;
                            ++i
                        ){ signature.push_back(semantic.argindex_to_symbol_map->find(i)->second.type_cache->value); }

                        // skip duplicated
                        if(stub_index.find(signature) != stub_index.end()){
                            continue;
                        }

                        // make function name
                        if(stub_count.find(*semantic.action) == stub_count.end()){
                            stub_count[*semantic.action] = 0;
                        }
                        std::size_t index = stub_count[*semantic.action];
                        stub_index[signature] = index;
                        stub_count[*semantic.action] = index + 1;

                        std::string function_name =
                            "call_" + lexical_cast(index) + "_" + semantic.action->to_string();
                        
                        // generate
                        {
                            std::stringstream ss;
                            for(std::size_t i = 0, i_length = semantic.argindex_to_symbol_map->size(); i < i_length; i++){
                                ss << ", arg_idx_" << i;
                            }
                            os << s_function("parser." + function_name, "nonterminal_index, base" + ss.str());
                        }

                        // semantic action
                        os << indent_1 << "let l:r = self.sa_." << *semantic.action << "(";
                        bool first = true;
                        for(std::size_t i = 0, i_length = semantic.argindex_to_symbol_map->size(); i < i_length; ++i){
                            if(first){ first = false; }else{ os << ", "; }
                            os << "self.get_arg(a:base, a:arg_idx_" << i << ")";
                        }
                        os << ")\n";

                        // automatic return value conversion
                        // os << indent_1 << "let l:v = 0\n" << indent_1 << "call self.sa_.upcast(l:v, l:r)\n";
                        os << indent_1 << "call self.pop_stack(a:base)\n";
                        os << indent_1
                            << "return self[self.stack_top().gotof](a:nonterminal_index, l:r)\n";
                        os << endfunction;
                        os << "\n";
                    }
                }
            }

            // states handler
            for(
                auto table_iter = ref_parsing_table.begin(), table_end = ref_parsing_table.end();
                table_iter != table_end;
                ++table_iter
            ){
                std::size_t state_num = table_iter - ref_parsing_table.begin();
                auto goto_fns(*table_iter->goto_fns);

                // gotof header
                os << "function! s:parser.gotof_"
                   << state_num
                   << "(nonterminal_index, v)\n";

                // gotof dispatcher
                std::stringstream ss;
                bool output_switch = false;
                std::set<std::size_t> generated;
                for(
                    auto expression_iter = ref_expression_set.begin(), expression_end = ref_expression_set.end();
                    expression_iter != expression_end;
                    ++expression_iter
                ){
                    auto &expression(*expression_iter);
                    std::size_t nonterminal_index;
                    {
                        scanner::scanner_type::symbol_type symbol;
                        symbol.value = scanner.term_to_token_map.find(expression.lhs)->second;
                        nonterminal_index = std::distance(
                            scanner.nonterminal_symbol_map.begin(),
                            scanner.nonterminal_symbol_map.find(symbol)
                        );
                    }

                    if(generated.find(nonterminal_index) != generated.end()){
                        continue;
                    }

                    auto goto_lhs_iter = goto_fns.find(expression.lhs);
                        
                    if(goto_lhs_iter != goto_fns.end()){
                        auto &goto_lhs(*goto_lhs_iter);
                        if(output_switch){
                            ss << indent_1 << "elseif a:nonterminal_index == " << nonterminal_index << "\n";
                        }else{
                            ss << indent_1 << "if a:nonterminal_index == " << nonterminal_index << "\n";
                        }
                        
                        ss << indent_1 << indent_1 << "return self.push_stack('state_"
                           << goto_lhs.second
                           << "', 'gotof_"
                           << goto_lhs.second
                           << "', a:v)\n";

                        output_switch = true;
                        generated.insert(nonterminal_index);
                    }
                }
                ss << indent_1 << "else\n"
                   << indent_1 << indent_1 << "return 0\n";
                ss << indent_1 << "endif\n";
                if(output_switch){
                    os << ss.str();
                }else{
                    os << indent_1 << "return 1\n";
                }

                // gotof footer
                os << "endfunction\n\n";

                // state header
                os << "function! s:parser.state_"
                   << state_num
                   << "(t, value)\n";

                // dispatcher header
                os << indent_1 << "if ";

                // reduce action cache
                typedef boost::tuple<
                    std::vector<string_iter_pair_type>,
                    std::size_t,
                    std::size_t,
                    std::vector<std::size_t>
                > reduce_action_cache_key_type;
                typedef std::map<
                    reduce_action_cache_key_type,
                    std::vector<std::string>
                > reduce_action_cache_type;
                reduce_action_cache_type reduce_action_cache;
                bool elseif_flag = false;

                // action table
                auto &actions(*table_iter->actions);
                for(
                    auto action_iter = actions.begin(), action_end = actions.end();
                    action_iter != action_end;
                    ++action_iter
                ){
                    auto &action(*action_iter);

                    // action header
                    std::string case_tag;
                    {
                        auto find_ret = scanner.term_to_token_map.find(action.first);
                        if(find_ret == scanner.term_to_token_map.end()){
                            case_tag = "s:token." + scanner.namespace_token.to_string() + "_0";
                        }else{
                            auto &token_name(find_ret->second.value);
                            case_tag = "s:" + scanner.namespace_token.to_string() + "." + token_name.to_string();
                        }
                    }

                    // action
                    switch(action.second.action_kind){
                    case action_shift:
                        if(action_iter != actions.begin()){
                            os << indent_1 << "elseif ";
                        }
                        os << "a:t == " << case_tag << "\n";
                        os << indent_1 << indent_1 << "call self.push_stack("
                           << "'state_" << action.second.action_number << "', "
                           << "'gotof_" << action.second.action_number << "', "
                           << "a:value)\n"
                           << indent_1 << indent_1 << "return 0\n";
                        elseif_flag = true;
                        break;

                    case action_reduce:
                        {
                            std::size_t base = action.second.item->rhs->size();

                            auto &item(*action.second.item);

                            std::size_t nonterminal_index;
                            {
                                scanner::scanner_type::symbol_type symbol;
                                symbol.value = scanner.term_to_token_map.find(item.lhs)->second;
                                nonterminal_index = std::distance(
                                    scanner.nonterminal_symbol_map.begin(),
                                    scanner.nonterminal_symbol_map.find(symbol)
                                );
                            }

                            if(item.rhs->semantic.action){
                                auto semantic_aciton = *item.rhs->semantic.action;
                                    
                                std::vector<string_iter_pair_type> signature = make_signature(
                                    *item.rhs->semantic.action,
                                    *item.rhs->semantic.type,
                                    *item.rhs->semantic.argindex_to_symbol_map
                                );

                                std::vector<std::size_t> arg_indices;
                                auto &argindex_to_symbol_map(*item.rhs->semantic.argindex_to_symbol_map);
                                for(
                                    std::size_t i = 0, i_length = argindex_to_symbol_map.size();
                                    i < i_length;
                                    ++i
                                ){ arg_indices.push_back(argindex_to_symbol_map.find(i)->second.src_index); }

                                reduce_action_cache_key_type key =
                                    boost::make_tuple(
                                        signature,
                                        nonterminal_index,
                                        base,
                                        arg_indices
                                    );

                                reduce_action_cache[key].push_back(case_tag);
                            }else{
                                os << indent_1 << "elseif a:t == " << case_tag << "\n";
                                os << indent_1 << indent_1 << "self.pop_stack("
                                   << base
                                   << ")\n";
                                os << "let l:dummy";
                                os << indent_1 << indent_1
                                   << "return self[self.stack_top().gotof](" << nonterminal_index << ", l:dummy)\n";
                                elseif_flag = true;
                            }
                        }
                        break;

                    case action_acc:
                        os << indent_1 << "elseif a:t == " << case_tag << "\n";

                        os << indent_1 << indent_1 << "let self.accepted_ = 1\n"
                           << indent_1 << indent_1
                           << "let self.accepted_value = self.get_arg(1, 0)\n" // implicit root
                           << indent_1 << indent_1 << "return 0\n";
                        elseif_flag = true;
                        break;

                    default:
                        os << indent_1 << "elseif a:t == " << case_tag << "\n";
                        os << indent_1 << indent_1 << "self.sa_.syntax_error();\n";
                        os << indent_1 << indent_1 << "let self.error_ = 1\n"; 
                        os << indent_1 << indent_1 << "return 0\n";
                        elseif_flag = true;
                        break;
                    }

                    // action footer
                }

                // flush reduce action cache
                for(
                    auto iter = reduce_action_cache.begin(), end = reduce_action_cache.end();
                    iter != end;
                    ++iter
                ){
                    auto &key(iter->first);
                    auto &cases(iter->second);

                    auto &signature(key.get<0>());
                    std::size_t nonterminal_index = key.get<1>();
                    std::size_t base = key.get<2>();
                    auto &arg_indices(key.get<3>());

                    if(elseif_flag){
                        os << indent_1 << "elseif ";
                    }
                    for(std::size_t i = 0, i_length = cases.size(); i < i_length; ++i){
                        if(i != 0){ os << " || "; }
                        os << "a:t == " << cases[i];
                    }
                    os << "\n";

                    std::size_t index = stub_index[signature];
                    std::string function_name = "call_" + lexical_cast(index) + "_" + signature[0].to_string();
                        
                    os << indent_1 << indent_1 << "return "
                       << "self." << function_name << "(" << nonterminal_index << ", " << base;
                    for(
                        auto arg_indices_iter = arg_indices.begin(), arg_indices_end = arg_indices.end();
                        arg_indices_iter != arg_indices_end;
                        ++arg_indices_iter
                    ){ os << ", " << (*arg_indices_iter); }
                    os << ")\n";
                }

                // dispatcher footer
                os << indent_1 << "else\n"
                   << indent_1 << indent_1 << "call self.sa_.syntax_error()\n"
                   << indent_1 << indent_1 << "let self.error_ = 1\n"
                   << indent_1 << indent_1 << "return 0\n";
                os << indent_1 << "endif\n";

                // state footer
                os << "endfunction\n\n";
            }

            // parser class footer
            // * null *
        }
    }
}

