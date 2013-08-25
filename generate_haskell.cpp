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
    namespace scanner{
        namespace semantic_action{
            typedef scanner_type::token_type token_type;
            typedef scanner_type::semantic_type semantic_type;
            scanner_type::symbol_type make_symbol(const token_type &token, term_type term);
        }
    }

    namespace target{
        typedef scanner::scanner_type::nonterminal_symbol_data_type::rhs_type::argindex_to_symbol_map_type argindex_to_symbol_map_type;

        std::vector<string_iter_pair_type> make_signature(
            const string_iter_pair_type &semantic_action,
            const string_iter_pair_type &return_type,
            const argindex_to_symbol_map_type &argindex_to_symbol_map
        );

        void target_type::generate_haskell(
            std::ostream &os,
            const commandline_options_type &commandline_options,
            const scanner::scanner_type &scanner
        ){
            indent_type indent_1(commandline_options.indent(), 1);

            // root semantic type 'Type'
            const std::string type = scanner.nonterminal_symbol_map.find(scanner::semantic_action::make_symbol(scanner.first_nonterminal_symbol.value, scanner.first_nonterminal_symbol.value.term))->second.type.value.to_string();

            // data type 'Token'
            const std::string data_type_token = !scanner.namespace_token.empty() ? scanner.namespace_token.to_string() : "Token";

            // defaultValue
            const std::string default_value = scanner.default_semantic_action.empty() ? std::string("0") : scanner.default_semantic_action.to_string();

            // module
            {
                std::size_t n = 0;
                std::string export_targets;
                
                // first target, data type 'Token'
                if(!scanner.external_token){
                    export_targets = data_type_token + "(..)";
                    ++n;
                }

                // ['comma']
                if(n > 0){
                    export_targets += ", ";
                }

                os << "module " << scanner.namespace_grammar << " (" << export_targets << "ParsingData(..), initParsingData, post, resultAccept, resultError) where\n";
            }

            // [import SemanticData]
            os << "import SemanticData\n\n";

            // [type Value = ...]
            os << "type Value = " << type << "\n";

            // [data 'Token' = ...]
            std::size_t case_tag_norm = 3;
            if(!scanner.external_token){
                os << "data " << data_type_token << " = End";
                for(std::size_t i = 0; i < scanner.number_to_token_map.size(); ++i){
                    auto find_ret(scanner.number_to_token_map.find(i)->second);
                    os << " | " << find_ret.value;
                    if(case_tag_norm < static_cast<std::size_t>(find_ret.value.size())){
                        case_tag_norm = find_ret.value.size();
                    }
                }
                os << "\n" << indent_1 << "deriving (Enum, Eq)\n\n";
            }

            os  << "data StackFrame = StackFrame {\n"
                << indent_1 << "state :: ParsingData -> Token -> Value -> (ParsingData, Bool),\n"
                << indent_1 << "gotof :: ParsingData -> Int   -> Value -> (ParsingData, Bool),\n"
                << indent_1 << "value :: Value\n"
                << "}\n"
                << "\n"
                << "data Stack = Stack {\n"
                << indent_1 << "stack :: [StackFrame],\n"
                << indent_1 << "tmp   :: [StackFrame],\n"
                << indent_1 << "gap   :: Int\n"
                << "}\n"
                << "\n"
                << "data ParsingData = ParsingData {\n"
                << indent_1 << "accepted      :: Bool,\n"
                << indent_1 << "parsingError  :: Bool,\n"
                << indent_1 << "acceptedValue :: Value,\n"
                << indent_1 << "parsingStack  :: Stack\n"
                << "}\n"
                << "\n"
                << "resetTmp  s   = Stack (stack s) [] (length (stack s))\n"
                << "commitTmp s   = Stack ((tmp s) ++ (drop ((length (stack s)) - (gap s)) (stack s))) (tmp s) (gap s)\n"
                << "push      s f = Stack (stack s) (f : (tmp s)) (gap s)\n"
                << "\n"
                << "pop s n =\n"
                << indent_1 << "if (length (tmp s)) < n\n"
                << indent_1 << indent_1 << "then Stack (stack s) [] ((gap s) - (n - (length (tmp s))))\n"
                << indent_1 << indent_1 << "else Stack (stack s) (drop n (tmp s)) (gap s)\n"
                << "\n"
                << "top s =\n"
                << indent_1 << "if (length (tmp s)) > 0\n"
                << indent_1 << indent_1 << "then head (tmp s)\n"
                << indent_1 << indent_1 << "else (stack s) !! ((length (stack s)) - 1 - ((gap s) - 1))\n"
                << "\n"
                << "getArg s b i =\n"
                << indent_1 << "let n = length (tmp s)\n"
                << indent_1 << "in if (b - i) <= n\n"
                << indent_1 << indent_1 << "then (tmp s)   !! (n - 1 - (n - (b - i)))\n"
                << indent_1 << indent_1 << "else (stack s) !! ((length (stack s)) - 1 - (gap s) + (b - n) - i)\n"
                << "\n"
                << "clear       s     = Stack [] (tmp s) (gap s)\n"
                << "stackTop    p     = top (parsingStack p)\n"
                << "getStackArg p b i = value (getArg (parsingStack p) b i)\n"
                << "\n"
                << "pushStack      p s g v = ParsingData (accepted p) (parsingError p) (acceptedValue p) (push (parsingStack p) (StackFrame s g v))\n"
                << "popStack       p n     = ParsingData (accepted p) (parsingError p) (acceptedValue p) (pop (parsingStack p) n)\n"
                << "clearStack     p       = ParsingData (accepted p) (parsingError p) (acceptedValue p) (clear (parsingStack p))\n"
                << "resetTmpStack  p       = ParsingData (accepted p) (parsingError p) (acceptedValue p) (resetTmp (parsingStack p))\n"
                << "commitTmpStack p       = ParsingData (accepted p) (parsingError p) (acceptedValue p) (commitTmp (parsingStack p))\n"
                << "\n"
                << "reset           = commitTmpStack (pushStack (ParsingData False False " << default_value << " (Stack [] [] 0)) state0 gotof0 " << default_value << ")\n"
                << "initParsingData = reset\n"
                << "\n"
                << "iteration p t v =\n"
                << indent_1 << "let n = (state (stackTop p)) p t v\n"
                << indent_1 << "    tFirst  (a, _) = a\n"
                << indent_1 << "    tSecond (_, b) = b\n"
                << indent_1 << "in if tSecond n\n"
                << indent_1 << indent_1 << "then iteration (tFirst n) t v\n"
                << indent_1 << indent_1 << "else (tFirst n)\n"
                << "\n"
                << "wrappedCommit p =\n"
                << indent_1 << "let p' = commitTmpStack p\n"
                << indent_1 << "in p'\n"
                << "\n"
                << "post p t v =\n"
                << indent_1 << "let p' = iteration (commitTmpStack p) t v\n"
                << indent_1 << "in if parsingError p'\n"
                << indent_1 << indent_1 << "then p'\n"
                << indent_1 << indent_1 << "else wrappedCommit p'\n"
                << "\n"
                << "resultAccept p = accepted p\n"
                << "resultError  p = parsingError p\n"
                << "\n";

            // c module
            std::ostringstream ms;

            std::map<std::vector<string_iter_pair_type>, std::size_t> stub_index;
            {
                std::map<string_iter_pair_type, std::size_t> stub_count;
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
                        std::vector<string_iter_pair_type> signature;
                        signature.push_back(*semantic.action);
                        signature.push_back(*semantic.type);

                        for(
                            std::size_t i = 0, i_length = semantic.argindex_to_symbol_map->size();
                            i < i_length;
                            ++i
                        ){ signature.push_back(semantic.argindex_to_symbol_map->find(i)->second.type_cache->value); }

                        if(stub_index.find(signature) != stub_index.end()){
                            continue;
                        }

                        if(stub_count.find(*semantic.action) == stub_count.end()){
                            stub_count[*semantic.action] = 0;
                        }

                        std::size_t index = stub_count[*semantic.action];
                        stub_index[signature] = index;
                        stub_count[*semantic.action] = index + 1;

                        std::string semantic_action_name = semantic.action->to_string();
                        if(semantic_action_name.empty()){
                            semantic_action_name = "0";
                        }
                        
                        std::string function_name = "call" + lexical_cast(index) + semantic_action_name;
                        os << function_name << " p i b";
                        for(std::size_t i = 0, i_length = semantic.argindex_to_symbol_map->size(); i < i_length; i++){
                            os << " i" << i;
                        }
                        if(!semantic.action->empty()){
                            os  << " =\n"
                                << indent_1 << "let p' = popStack p b\n"
                                << indent_1 << "in (gotof (stackTop p')) p' i (" << *semantic.action;

                            ms  << ", " << *semantic.action;

                            for(std::size_t i = 0, i_length = semantic.argindex_to_symbol_map->size(); i < i_length; ++i){
                                auto &arg_data(semantic.argindex_to_symbol_map->find(i)->second);
                                os  << " (getStackArg p b i" << i << ")";
                            }

                            os  << ")\n"
                                << "\n";
                        }else{
                            os  << " = (gotof (stackTop p)) p i " << default_value << "\n";
                        }
                    }
                }
            }

            for(
                auto table_iter = ref_parsing_table.begin(), table_end = ref_parsing_table.end();
                table_iter != table_end;
                ++table_iter
            ){
                std::size_t state_num = table_iter - ref_parsing_table.begin();
                auto goto_fns(*table_iter->goto_fns);

                os  << "gotof" << state_num << " p i v =\n"
                    << indent_1 << "case i of\n";

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
                        ss  << indent_1 << indent_1
                            << nonterminal_index
                            << " -> (pushStack p state" << goto_lhs.second
                            << " gotof" << goto_lhs.second << " v, True)\n";
                        output_switch = true;
                        generated.insert(nonterminal_index);
                    }
                }

                ss  << indent_1 << indent_1 << "_ -> (p, False)\n";
                if(output_switch){
                    os  << ss.str();
                }else{
                    os  << indent_1 << indent_1 << "_ -> (p, True)\n";
                }
                os  << "\n";

                os  << "state" << state_num << " p t v =\n"
                    << indent_1 << "case t of\n";

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

                auto &actions(*table_iter->actions);
                std::size_t digit_action_number = 0;
                for(
                    auto action_iter = actions.begin(), action_end = actions.end();
                    action_iter != action_end;
                    ++action_iter
                ){
                    auto &action(*action_iter);
                    std::size_t n = lexical_cast(action.second.action_number).size();
                    if(n > digit_action_number){ digit_action_number = n; }
                }

                for(
                    auto action_iter = actions.begin(), action_end = actions.end();
                    action_iter != action_end;
                    ++action_iter
                ){
                    auto &action(*action_iter);

                    std::string f_act_num = lexical_cast(action.second.action_number);
                    f_act_num += std::string(digit_action_number - f_act_num.size(), ' ');

                    std::string case_tag;
                    {
                        auto find_ret = scanner.term_to_token_map.find(action.first);
                        if(find_ret == scanner.term_to_token_map.end()){
                            case_tag = "End";
                        }else{
                            case_tag = find_ret->second.value.to_string();
                        }
                    }

                    switch(action.second.action_kind){
                    case action_shift:
                        os  << indent_1 << indent_1
                            << case_tag << std::string(case_tag_norm - case_tag.size(), ' ') << " -> "
                            << "(pushStack p state" << f_act_num << " gotof" << f_act_num << " v, False)\n";
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
                                os  << indent_1 << indent_1
                                    << case_tag << std::string(case_tag_norm - case_tag.size(), ' ') << " -> (gotof (stackTop p)) p i "
                                    << (scanner.default_semantic_action.empty() ? std::string("0") : scanner.default_semantic_action.to_string())
                                    << "\n";
                            }
                        }
                        break;

                    case action_acc:
                        os  << indent_1 << indent_1
                            << case_tag << std::string(case_tag_norm - case_tag.size(), ' ') << " -> (ParsingData True (parsingError p) (getStackArg p 1 0) (parsingStack p), False)"
                            << "\n";
                        break;

                    default:
                        os  << indent_1 << indent_1
                            << case_tag << std::string(case_tag_norm - case_tag.size(), ' ') << " -> (ParsingData (accepted p) True (acceptedValue p) (parsingStack p), False)"
                            << "\n";
                        break;
                    }
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

                    for(std::size_t i = 0, i_length = cases.size(); i < i_length; ++i){
                        os  << indent_1 << indent_1 << cases[i] << std::string(case_tag_norm - cases[i].size(), ' ') << " -> ";

                        std::size_t index = stub_index[signature];
                        std::string function_name = "call" + lexical_cast(index);
                        std::string call_target = signature[0].to_string();
                        if(call_target.empty()){
                            call_target = "0";
                        }

                        function_name += call_target;

                        os  << function_name << " p " << nonterminal_index << " " << base;
                        for(
                            auto arg_indices_iter = arg_indices.begin(), arg_indices_end = arg_indices.end();
                            arg_indices_iter != arg_indices_end;
                            ++arg_indices_iter
                        ){
                            os  << " " << (*arg_indices_iter);
                        }
                        os  << "\n";
                    }
                }

                os  << indent_1 << indent_1 << "_" << std::string(case_tag_norm - 1, ' ') << " -> (ParsingData (accepted p) True (acceptedValue p) (parsingStack p), False)\n"
                    << "\n";
            }

            // [-- module SemanticData ('Type', 'Init', ...) where]
            os  << "-- module SemanticData (" << type;
            if(!scanner.default_semantic_action.empty()){
                os  << ", " << scanner.default_semantic_action;
            }
            os  << ms.str() << ") where\n";
        }
    }
}
