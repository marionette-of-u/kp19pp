#include <list>
#include <sstream>
#include <cstdio>
#include <cctype>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include "target.hpp"
#include "indent.hpp"

namespace kp19pp{
    namespace target{
        namespace{
            typedef scanner::scanner_type::nonterminal_symbol_data_type::rhs_type::argindex_to_symbol_map_type argindex_to_symbol_map_type;

            std::vector<string_iter_pair_type> make_signature(
                const string_iter_pair_type &semantic_action,
                const string_iter_pair_type &return_type,
                const argindex_to_symbol_map_type &argindex_to_symbol_map
            ){
                std::vector<string_iter_pair_type> signature;

                // function name
                signature.push_back(semantic_action);

                // return value
                signature.push_back(return_type);

                //arguments
                for(std::size_t i = 0, i_length = argindex_to_symbol_map.size(); i < i_length; ++i){
                    signature.push_back(argindex_to_symbol_map.find(i)->second.type_cache->value);
                }

                return std::move(signature);
            }
        }

        void target_type::generate_cpp(
            std::ostream &os,
            const commandline_options_type &commandline_options,
            const scanner::scanner_type &scanner
        ){
            indent_type indent_1(commandline_options.indent(), 1);

            std::string headername = commandline_options.ofile_name();
            for(std::string::iterator i = headername.begin(); i != headername.end(); ++i){
                if(!std::isalpha(*i) && !std::isdigit(*i)){
                    *i = '_';
                }else{
                    *i = std::toupper(*i);
                }
            }

            // once header
            os << "#ifndef " << headername << "_\n"
               << "#define " << headername << "_\n\n";

            // include
            os << "#include <cstdlib>\n";
            os << "#include <cassert>\n";
            os << "#include <vector>\n";
            os << "\n";

            // namespace header
            os << "namespace " << scanner.namespace_grammar << "{\n\n";

            std::string namespace_token = scanner.namespace_token.size() > 0 ? scanner.namespace_token.to_string() + std::string("_") : std::string("token_");

            if(!scanner.external_token){
                // token enumeration
                if(scanner.namespace_token.empty()){
                    os << "enum token{\n";
                }else{
                    os << "enum " << scanner.namespace_token << "{\n";
                }
                for(std::size_t i = 0; i < scanner.number_to_token_map.size(); ++i){
                    auto find_ret(scanner.number_to_token_map.find(i)->second);
                    os << indent_1 << namespace_token << find_ret.value << ",";
                    os << "\n";
                }
                os << indent_1 << namespace_token << "0\n"
                   << "};\n\n";
            }

            // stack class header

            // STL version
            os << "template<class T, int StackSize>\n"
               << "class stack{\n"
               << "public:\n"
               << indent_1 << "stack(){ gap_ = 0; }\n"
               << indent_1 << "~stack(){}\n"
               << indent_1 << "\n"
               << indent_1 << "void reset_tmp()\n"
               << indent_1 << "{\n"
               << indent_1 << indent_1 << "gap_ = stack_.size();\n"
               << indent_1 << indent_1 << "tmp_.clear();\n"
               << indent_1 << "}\n"
               << "\n"
               << indent_1 << "void commit_tmp()\n"
               << indent_1 << "{\n"
               << indent_1 << indent_1 << "// may throw\n"
               << indent_1 << indent_1 << "stack_.reserve(gap_ + tmp_.size());\n"
               << "\n"
               << indent_1 << indent_1 << "// expect not to throw\n"
               << indent_1 << indent_1 << "stack_.erase(stack_.begin() + gap_, stack_.end());\n"
               << indent_1 << indent_1 << "stack_.insert(stack_.end(), tmp_.begin(), tmp_.end());\n"
               << indent_1 << "}\n"
               << "\n"
               << indent_1 << "bool push(const T& f)\n"
               << indent_1 << "{\n"
               << indent_1 << indent_1 << "if(StackSize != 0 && StackSize <= stack_.size() + tmp_.size()){\n"
               << indent_1 << indent_1 << indent_1 << "return false;\n"
               << indent_1 << indent_1 << "}\n"
               << indent_1 << indent_1 << "tmp_.push_back(f);\n"
               << indent_1 << indent_1 << "return true;\n"
               << indent_1 << "}\n"
               << "\n"
               << indent_1 << "void pop(std::size_t n)\n"
               << indent_1 << "{\n"
               << indent_1 << indent_1 << "if(tmp_.size() < n){\n"
               << indent_1 << indent_1 << indent_1 << "n -= tmp_.size();\n"
               << indent_1 << indent_1 << indent_1 << "tmp_.clear();\n"
               << indent_1 << indent_1 << indent_1 << "gap_ -= n;\n"
               << indent_1 << indent_1 << "}else{\n"
               << indent_1 << indent_1 << indent_1 << "tmp_.erase(tmp_.end() - n, tmp_.end());\n"
               << indent_1 << indent_1 << "}\n"
               << indent_1 << "}\n"
               << "\n"
               << indent_1 << "const T& top()\n"
               << indent_1 << "{\n"
               << indent_1 << indent_1 << "if(!tmp_.empty()){\n"
               << indent_1 << indent_1 << indent_1 << "return tmp_.back();\n"
               << indent_1 << indent_1 << "} else {\n"
               << indent_1 << indent_1 << indent_1 << "return stack_[gap_ - 1];\n"
               << indent_1 << indent_1 << "}\n"
               << indent_1 << "}\n"
               << "\n"
               << indent_1 << "const T& get_arg(std::size_t base, std::size_t index)\n"
               << indent_1 << "{\n"
               << indent_1 << indent_1 << "std::size_t n = tmp_.size();\n"
               << indent_1 << indent_1 << "if(base - index <= n){\n"
               << indent_1 << indent_1 << indent_1 << "return tmp_[n - (base - index)];\n"
               << indent_1 << indent_1 << "}else{\n"
               << indent_1 << indent_1 << indent_1 << "return stack_[gap_ - (base - n) + index];\n"
               << indent_1 << indent_1 << "}\n"
               << indent_1 << "}\n"
               << "\n"
               << indent_1 << "void clear()\n"
               << indent_1 << "{\n"
               << indent_1 << indent_1 << "stack_.clear();\n"
               << indent_1 << "}\n"
               << "\n"
               << "private:\n"
               << indent_1 << "std::vector<T> stack_;\n"
               << indent_1 << "std::vector<T> tmp_;\n"
               << indent_1 << "std::size_t gap_;\n"
               << "\n"
               << "};\n\n";

            // parser class header
            std::string default_stacksize = "0";
            std::string template_parameters = "class Value, class SemanticAction, int StackSize = " + default_stacksize;
            if(scanner.external_token){
                template_parameters = "class Token, " + template_parameters;
            }

            os << "template<" << template_parameters << ">\n";
            os <<  "class parser {\n";

            // public interface
            os << "public:\n"
               << indent_1 << "typedef " << (scanner.external_token ? "Token" : scanner.namespace_token.size() > 0 ? scanner.namespace_token.to_string().c_str() : "token") << " token_type;\n"
               << indent_1 << "typedef Value value_type;\n"
               << "\n"
               << "public:\n"
               << indent_1 << "parser(SemanticAction& sa) : sa_(sa){ reset(); }\n"
               << "\n"
               << indent_1 << "void reset()\n"
               << indent_1 << "{\n"
               << indent_1 << indent_1 << "error_ = false;\n"
               << indent_1 << indent_1 << "accepted_ = false;\n"
               << indent_1 << indent_1 << "clear_stack();\n"
               << indent_1 << indent_1 << "reset_tmp_stack();\n"
               << indent_1 << indent_1 << "if(push_stack(&parser::state_0, &parser::gotof_0, value_type())){\n"
               << indent_1 << indent_1 << indent_1 << "commit_tmp_stack();\n"
               << indent_1 << indent_1 << "}else{\n"
               << indent_1 << indent_1 << indent_1 << "sa_.stack_overflow();\n"
               << indent_1 << indent_1 << indent_1 << "error_ = true;\n"
               << indent_1 << indent_1 << "}\n"
               << indent_1 << "}\n"
               << "\n"
               << indent_1 << "bool post(token_type token, const value_type& value)\n"
               << indent_1 << "{\n"
               << indent_1 << indent_1 << "assert(!error_);\n"
               << indent_1 << indent_1 << "reset_tmp_stack();\n"
               << indent_1 << indent_1 << "while((this->*(stack_top()->state))(token, value)); // may throw\n"
               << indent_1 << indent_1 << "if(!error_){\n"
               << indent_1 << indent_1 << indent_1 << "commit_tmp_stack();\n"
               << indent_1 << indent_1 << "}\n"
               << indent_1 << indent_1 << "return accepted_ || error_;\n"
               << indent_1 << "}\n"
               << "\n"
               << indent_1 << "bool accept(value_type& v)\n"
               << indent_1 << "{\n"
               << indent_1 << indent_1 << "assert(accepted_);\n"
               << indent_1 << indent_1 << "if(error_){ return false; }\n"
               << indent_1 << indent_1 << "v = accepted_value_;\n"
               << indent_1 << indent_1 << "return true;\n"
               << indent_1 << "}\n"
               << "\n"
               << indent_1 << "bool error(){ return error_; }\n\n";

            // implementation
            os << "private:\n";
            if(scanner.external_token){
                os << indent_1 << "typedef parser<Token, Value, SemanticAction, StackSize> self_type;\n";
            }else{
                os << indent_1 << "typedef parser<Value, SemanticAction, StackSize> self_type;\n";
            }
            os << indent_1 << "typedef bool (self_type::*state_type)(token_type, const value_type&);\n"
               << indent_1 << "typedef bool ( self_type::*gotof_type )(int, const value_type&);\n"
               << "\n"
               << indent_1 << "bool            accepted_;\n"
               << indent_1 << "bool            error_;\n"
               << indent_1 << "value_type      accepted_value_;\n"
               << indent_1 << "SemanticAction& sa_;\n"
               << "\n"
               << indent_1 << "struct stack_frame{\n"
               << indent_1<< indent_1 << "state_type state;\n"
               << indent_1<< indent_1 << "gotof_type gotof;\n"
               << indent_1<< indent_1 << "value_type value;\n"
               << "\n"
               << indent_1<< indent_1 << "stack_frame(state_type s, gotof_type g, const value_type& v)\n"
               << indent_1<< indent_1 << indent_1 << ": state(s), gotof(g), value(v){}\n"
               << indent_1 << "};\n\n";

            // stack operation
            os << indent_1 << "stack<stack_frame, StackSize> stack_;\n"
               << indent_1 << "bool push_stack(state_type s, gotof_type g, const value_type& v)\n"
               << indent_1 << "{\n"
               << indent_1 << indent_1 << "bool f = stack_.push(stack_frame(s, g, v));\n"
               << indent_1 << indent_1 << "assert(!error_);\n"
               << indent_1 << indent_1 << "if(!f){\n"
               << indent_1 << indent_1 << indent_1 << "error_ = true;\n"
               << indent_1 << indent_1 << indent_1 << "sa_.stack_overflow();\n"
               << indent_1 << indent_1 << "}\n"
               << indent_1<< indent_1 << "return f;\n"
               << indent_1 << "}\n"
               << "\n"
               << indent_1 << "void pop_stack(std::size_t n)\n"
               << indent_1 << "{\n"
               << indent_1 << indent_1 << "stack_.pop(n);\n"
               << indent_1 << "}\n"
               << "\n"
               << indent_1 << "const stack_frame* stack_top()\n"
               << indent_1 << "{\n"
               << indent_1 << indent_1 << "return &stack_.top();\n"
               << indent_1 << "}\n"
               << "\n"
               << indent_1 << "const value_type& get_arg(std::size_t base, std::size_t index)\n"
               << indent_1 << "{\n"
               << indent_1 << indent_1 << "return stack_.get_arg(base, index).value;\n"
               << indent_1 << "}\n"
               << "\n"
               << indent_1 << "void clear_stack()\n"
               << indent_1 << "{\n"
               << indent_1 << indent_1 << "stack_.clear();\n"
               << indent_1 << "}\n"
               << "\n"
               << indent_1 << "void reset_tmp_stack()\n"
               << indent_1 << "{\n"
               << indent_1 << indent_1 << "stack_.reset_tmp();\n"
               << indent_1 << "}\n"
               << "\n"
               << indent_1 << "void commit_tmp_stack()\n"
               << indent_1 << "{\n"
               << indent_1 << indent_1 << "stack_.commit_tmp();\n"
               << indent_1 << "}\n\n";

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
                        if(semantic.action->empty()){ continue; }

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
                        os << indent_1 << "bool " << function_name << "(int nonterminal_index, int base";
                        for(std::size_t i = 0, i_length = semantic.argindex_to_symbol_map->size(); i < i_length; i++){
                            os << ", int arg_index" << i;
                        }
                        os << ")\n";

                        os << indent_1 << "{\n";
                        // automatic argument conversion
                        for(std::size_t i = 0, i_length = semantic.argindex_to_symbol_map->size(); i < i_length; ++i){
                            auto &arg_data(semantic.argindex_to_symbol_map->find(i)->second);
                            os << indent_1
                               << indent_1
                               << arg_data.type_cache->value
                               << " arg"
                               << i
                               << "; sa_.downcast(arg"
                               << i
                               << ", get_arg(base, arg_index"
                               << i
                               << "));\n";
                        }

                        // semantic action
                        os << indent_1 << indent_1 << *semantic.type << " r = sa_." << *semantic.action << "(";
                        bool first = true;
                        for(std::size_t i = 0, i_length = semantic.argindex_to_symbol_map->size(); i < i_length; ++i){
                            if(first){ first = false; }else{ os << ", "; }
                            os << "arg" << i;
                        }
                        os << ");\n";

                        // automatic return value conversion
                        os << indent_1 << indent_1
                           << "value_type v; sa_.upcast(v, r);\n";
                        os << indent_1 << indent_1 << "pop_stack(base);\n";
                        os << indent_1 << indent_1
                           << "return (this->*(stack_top()->gotof))(nonterminal_index, v);\n";
                        os << indent_1 << "}\n\n";
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
                os << indent_1
                    << "bool gotof_"
                    << state_num
                    << "(int nonterminal_index, const value_type& v)\n"
                    << indent_1
                    << "{\n";

                // gotof dispatcher
                std::stringstream ss;
                ss << indent_1
                    << indent_1
                    << "switch(nonterminal_index){\n";
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
                        ss << indent_1
                            << indent_1
                            << "case "
                            << nonterminal_index
                            << ": return push_stack(&parser::state_"
                            << goto_lhs.second
                            << ", &parser::gotof_"
                            << goto_lhs.second
                            << ", v);\n";
                        output_switch = true;
                        generated.insert(nonterminal_index);
                    }
                }
                ss << indent_1 << indent_1 << "default: assert(0); return false;\n";
                ss << indent_1 << indent_1 << "}\n";
                if(output_switch){
                    os << ss.str();
                }else{
                    os << indent_1 << indent_1 << "assert(0);\n"
                       << indent_1 << indent_1 << "return true;\n";
                }

                // gotof footer
                os << indent_1 << "}\n\n";

                // state header
                os << indent_1
                    << "bool state_"
                    << state_num
                    << "(token_type token, const value_type& value)\n"
                    << indent_1
                    << "{\n";

                // dispatcher header
                os << indent_1 << indent_1 << "switch(token){\n";

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
                            case_tag = namespace_token + "0";
                        }else{
                            auto &token_name(find_ret->second.value);
                            case_tag = namespace_token + token_name.to_string();
                        }
                    }

                    // action
                    switch(action.second.action_kind){
                    case action_shift:
                        os << indent_1 << indent_1 << "case " << case_tag << ":\n";

                        os << indent_1 << indent_1 << indent_1 << "// shift\n"
                            << indent_1 << indent_1 << indent_1 << "push_stack("
                            << "&parser::state_" << action.second.action_number << ", "
                            << "&parser::gotof_" << action.second.action_number << ", "
                            << "value);\n"
                            << indent_1 << indent_1 << indent_1 << "return false;\n";
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

                            if(!item.rhs->semantic.action->empty()){
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
                                ){
                                    arg_indices.push_back(
                                        lexical_cast(
                                            argindex_to_symbol_map.find(i)->second.number.value.to_string()
                                        )
                                    );
                                }

                                reduce_action_cache_key_type key =
                                    boost::make_tuple(
                                        signature,
                                        nonterminal_index,
                                        base,
                                        arg_indices
                                    );

                                reduce_action_cache[key].push_back(case_tag);
                            }else{
                                os << indent_1 << indent_1 << "case " << case_tag << ":\n";
                                os << indent_1 << indent_1 << indent_1 << "// reduce\n";

                                os << indent_1 << indent_1 << indent_1 << indent_1
                                    << "// run_semantic_action();\n";
                                os << indent_1 << indent_1 << indent_1 << indent_1 << "pop_stack("
                                    << base
                                    << ");\n";
                                os << indent_1 << indent_1 << indent_1 << indent_1
                                    << "return (this->*(stack_top()->gotof))("
                                    << nonterminal_index << ", value_type());\n";
                            }
                        }
                        break;

                    case action_acc:
                        os << indent_1 << indent_1 << "case " << case_tag << ":\n";

                        os << indent_1 << indent_1 << indent_1 << "// accept\n"
                            << indent_1 << indent_1 << indent_1 << "// run_semantic_action();\n"
                            << indent_1 << indent_1 << indent_1 << "accepted_ = true;\n"
                            << indent_1 << indent_1 << indent_1
                            << "accepted_value_ = get_arg(1, 0);\n" // implicit root
                            << indent_1 << indent_1 << indent_1 << "return false;\n";
                        break;

                    default:
                        os << indent_1 << indent_1 << "case " << case_tag << ":\n";

                        os << indent_1 << indent_1 << indent_1 << "sa_.syntax_error();\n";
                        os << indent_1 << indent_1 << indent_1 << "error_ = true;\n"; 
                        os << indent_1 << indent_1 << indent_1 << "return false;\n";
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

                    if(signature[0].empty()){ continue; }

                    for(std::size_t i = 0, i_length = cases.size(); i < i_length; ++i){
                        os << indent_1 << indent_1 << "case " << cases[i] << ":\n";
                    }

                    std::size_t index = stub_index[signature];
                    std::string function_name =
                        "call_" + lexical_cast(index) + "_" + signature[0].to_string();
                        
                    os << indent_1 << indent_1 << indent_1 << "return "
                        << function_name << "(" << nonterminal_index << ", " << base;
                    for(
                        auto arg_indices_iter = arg_indices.begin(), arg_indices_end = arg_indices.end();
                        arg_indices_iter != arg_indices_end;
                        ++arg_indices_iter
                    ){ os << ", " << (*arg_indices_iter); }
                    os << ");\n";
                }

                // dispatcher footer
                os << indent_1 << indent_1 << "default:\n"
                   << indent_1 << indent_1 << indent_1 << "sa_.syntax_error();\n"
                   << indent_1 << indent_1 << indent_1 << "error_ = true;\n"
                   << indent_1 << indent_1 << indent_1 << "return false;\n";
                os << indent_1 << indent_1 << "}\n";

                // state footer
                os << indent_1 << "}\n\n";
            }
            // parser class footer
            os << "};\n\n";

            // namespace footer
            os << "} // namespace " << scanner.namespace_grammar << "\n\n";

            // once footer
            os << "#endif // " << headername << "_\n";
        }
    }
}
