#ifndef KP19PP_HPP_
#define KP19PP_HPP_

#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <iterator>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <exception>
#include <fstream>
#include <climits>
#include <boost/timer.hpp>

namespace kp19pp{
    template<class Type>
    inline void hash_combine(std::size_t &h, const Type &value){
        // XOR combine
        h = h ^ std::hash<Type>()(value);
    }

    // 与えられたシンボルが非終端記号かを判定する functor
    // default は set から検索するだけ
    template<class ExpressionSetType>
    struct default_is_not_terminal{
        bool operator ()(
            const ExpressionSetType &expression_set,
            const typename ExpressionSetType::value_type &symbol
        ) const{
            typename ExpressionSetType::value_type dummy_lhs(symbol);
            return expression_set.find(dummy_lhs) != expression_set.end();
        }
    };

    // term を人間が読める形式に変換する
    // default は値をそのまま返すだけ
    template<class TermType>
    struct default_term_to_str{
        const TermType &operator ()(const TermType &term) const{
            return term;
        }
    };

    template<class Value, class Term>
    struct basic_token_type{
        typedef Value value_type;
        typedef Term term_type;

        basic_token_type()
            : value(), term(), char_num(0), line_num()
        {}

        basic_token_type(const value_type &value_, const term_type &term_, std::size_t char_num_, std::size_t line_num_) :
            value(value_),
            term(term_),
            char_num(char_num_),
            line_num(line_num_)
        {}

        basic_token_type(const basic_token_type &other)
            : value(other.value), term(other.term), char_num(other.char_num), line_num(other.line_num)
        {}

        basic_token_type(basic_token_type &&other)
            : value(std::move(other.value)), term(std::move(other.term)), char_num(std::move(other.char_num)), line_num(std::move(other.line_num))
        {}

        basic_token_type &operator =(const basic_token_type &other){
            value = other.value;
            term = other.term;
            char_num = other.char_num;
            line_num = other.line_num;
            return *this;
        }

        value_type value;
        term_type term;
        std::size_t char_num, line_num;
        struct hash{
            std::size_t operator ()(const basic_token_type &item) const{
                return typename value_type::hash()(item.value);
            }
        };
    };

    template<class Value, class Term, class Semantic, class EOSFn, class EPSFn>
    class lalr1_type{
    public:
        typedef Value value_type;
        typedef Term term_type;
        typedef Semantic semantic_type;
        typedef EOSFn eos_functor_type;
        typedef EPSFn eps_functor_type;
        typedef basic_token_type<Value, Term> token_type;

        typedef std::unordered_set<term_type> term_set_type;
        class expression_type{
        public:
            class rhs_type : public std::vector<term_type>{
            public:
                static const bool has_semantic = true;
                rhs_type() :
                    std::vector<term_type>(),
                    semantic(),
                    number_(0),
                    tag_()
                {}
                
                rhs_type(std::size_t arg_number, term_type arg_tag) :
                    std::vector<term_type>(),
                    semantic(),
                    number_(arg_number),
                    tag_(arg_tag)
                {}
                
                rhs_type(const rhs_type &other) :
                    std::vector<term_type>(other),
                    semantic(other.semantic),
                    number_(other.number_),
                    tag_(other.tag_)
                {}
                
                rhs_type(rhs_type &&other) :
                    std::vector<term_type>(other),
                    semantic(std::move(other.semantic)),
                    number_(std::move(other.number_)),
                    tag_(std::move(other.tag_))
                {}

                rhs_type &operator =(const rhs_type &other){
                    std::vector<term_type>::operator =(other);
                    semantic = other.semantic;
                    number_ = other.number_;
                    tag_ = other.tag_;
                }

                rhs_type &operator =(rhs_type &&other){
                    std::vector<term_type>::operator =(other);
                    semantic = std::move(other.semantic);
                    number_ = std::move(other.number_);
                    tag_ = std::move(other.tag_);
                }

                bool operator <(const rhs_type &other) const{
                    for(
                        auto iter = std::vector<term_type>::begin(), other_iter = other.begin(), end = std::vector<term_type>::end(), other_end = other.end();
                        iter != end || other_iter != other_end;
                        ++iter, ++other_iter
                    ){
                        if(iter == end){ return true; }
                        if(other_iter == other_end){ return false; }
                        if(*iter < *other_iter){ return true; }
                        if(*other_iter < *iter){ return false; }
                    }
                    return false;
                }

                void set_attribute(std::size_t arg_number){
                    number_ = arg_number;
                    tag_ = eps_functor_type()();
                }

                void set_attribute(std::size_t arg_number, const term_type &arg_tag){
                    number_ = arg_number;
                    tag_ = arg_tag;
                }

                std::size_t number() const{ return number_; }
                term_type tag() const{ return tag_; }

                struct hash{
                    std::size_t operator ()(const rhs_type &item) const{
                        std::size_t h = 0;
                        for(auto iter = item.begin(), end = item.end(); iter != end; ++iter){
                            hash_combine(h, *iter);
                        }
                        return h;
                    }
                };

                mutable semantic_type semantic;

            private:
                std::size_t number_;
                term_type tag_;
            };

            typedef std::unordered_set<rhs_type, typename rhs_type::hash> rhs_set_type;

        public:
            expression_type() : lhs(), rhs(){}
            expression_type(const expression_type &other) : lhs(other.lhs), rhs(other.rhs){}
            expression_type(expression_type &&other) : lhs(std::move(other.lhs)), rhs(std::move(other.rhs)){}
            expression_type(const term_type &a_lhs) : lhs(a_lhs), rhs(){}
            expression_type(const term_type &a_lhs, const term_type &a_rhs) : lhs(a_lhs), rhs(){
                rhs.insert(rhs_type()).first->push_back(a_rhs);
            }

            template<class Iter>
            expression_type(const term_type &a_lhs, Iter first, Iter last) : lhs(a_lhs), rhs(){
                for(; first != last; ++first){
                    rhs.insert(rhs_type()).first->push_back(*first);
                }
            }

            expression_type &operator =(const expression_type &other){
                lhs = other.lhs;
                rhs = other.rhs;
                return *this;
            }

            expression_type &operator =(expression_type &&other){
                lhs = std::move(other.lhs);
                rhs = std::move(other.rhs);
                return *this;
            }

            bool operator ==(const expression_type &other) const{
                return lhs == other.lhs;
            }

            struct hash{
                std::size_t operator ()(const expression_type &item) const{
                    std::size_t h = 0;
                    hash_combine(h, item.lhs);
                    return h;
                }
            };

        public:
            term_type lhs;
            rhs_set_type rhs;
        };

        typedef std::unordered_set<expression_type, typename expression_type::hash> expression_set_type;
        struct item_type{
            item_type() : lhs(), rhs(nullptr), pos(0), lookahead(){}
            item_type(const item_type &other)
                : lhs(other.lhs), rhs(other.rhs), pos(other.pos), lookahead(other.lookahead)
            {}

            item_type(item_type &&other)
                : lhs(std::move(other.lhs)), rhs(std::move(other.rhs)), pos(std::move(other.pos)), lookahead(std::move(other.lookahead))
            {}

            bool operator ==(const item_type &other) const{
                return lhs == other.lhs && rhs == other.rhs && pos == other.pos;
            }

            bool operator <(const item_type &other) const{
                if(lhs < other.lhs){
                    return true;
                }else if(lhs > other.lhs){
                    return false;
                }
                if(pos < other.pos){
                    return true;
                }else if(pos > other.pos){
                    return false;
                }
                if(*rhs < *other.rhs){
                    return true;
                }
                return false;
            }

            term_type lhs;
            const typename expression_type::rhs_type *rhs;
            std::size_t pos;
            // 先読み記号はマッチングに影響しない
            mutable std::set<term_type> lookahead;
        };

        struct item_set_type : public std::set<item_type>{
            typedef std::unordered_map<term_type, const item_set_type*> goto_map_type;
            const item_set_type mutable *mirror;
            mutable goto_map_type goto_map;
            item_set_type() :
                std::set<item_type>(), mirror(nullptr), goto_map()
            {}
            
            item_set_type(const item_set_type &other) :
                std::set<item_type>(other), mirror(other.mirror), goto_map(other.goto_map)
            {}

            item_set_type(item_set_type &&other) :
                std::set<item_type>(other), mirror(std::move(other.mirror)), goto_map(std::move(other.goto_map))
            {}

            item_set_type &operator =(const item_set_type &other){
                std::set<item_type>::operator =(other);
                mirror = other.mirror;
                goto_map = other.goto_map;
                return *this;
            }
        };

        typedef std::set<item_set_type> items_set_type;
        typedef std::unordered_map<term_type, std::unordered_set<term_type>> fset_type;
        struct action_type{
            bool operator ==(const action_type &other) const{
                return first == other.first && second == other.second && *item == *other.item;
            }

            bool operator <(const action_type &other) const{
                if(first < other.first){
                    return true;
                }else if(first > other.first){
                    return false;
                }
                if(second < other.second){
                    return true;
                }else if(second > other.second){
                    return false;
                }
                if(*item < *other.item){
                    return true;
                }
                return false;
            }

            std::size_t first, second;
            const item_type *item;
            struct hash{
                std::size_t operator ()(const action_type &item) const{
                    std::size_t h = 0;
                    hash_combine(h, item.first);
                    hash_combine(h, item.second);
                    hash_combine(h, item_type::hash()(*item.item));
                    return h;
                }
            };
        };
        
        struct actions_type : public std::map<term_type, action_type>{
            actions_type() :
                std::map<term_type, action_type>()
            {}

            actions_type(const actions_type &other) :
                std::map<term_type, action_type>(other)
            {}
                
            actions_type(actions_type &&other) :
                std::map<term_type, action_type>(other)
            {}
                
            actions_type &operator =(const actions_type &other){
                std::map<term_type, action_type>::operator =(other);
                return *this;
            }

            actions_type &operator =(actions_type &&other){
                std::map<term_type, action_type>::operator =(other);
                return *this;
            }

            struct hash{
                std::size_t operator ()(const actions_type &item) const{
                    std::size_t h = 0;
                    for(auto iter = item.begin(), end = item.end(); iter != end; ++iter){
                        auto &a(*iter);
                        hash_combine(h, a.first);
                        hash_combine(h, action_type::hash()(a.second));
                    }
                    return h;
                }
            };
        };

        struct goto_fns_type : public std::map<term_type, std::size_t>{
            goto_fns_type() :
                std::map<term_type, std::size_t>()
            {}
            
            goto_fns_type(const goto_fns_type &other) :
                std::map<term_type, std::size_t>(other)
            {}

            goto_fns_type(goto_fns_type &&other) :
                std::map<term_type, std::size_t>(other)
            {}

            goto_fns_type &operator =(const goto_fns_type &other){
                std::map<term_type, std::size_t>::operator =(other);
                return *this;
            }

            goto_fns_type &operator =(goto_fns_type &&other){
                std::map<term_type, std::size_t>::operator =(other);
                return *this;
            }

            struct hash{
                std::size_t operator ()(const goto_fns_type &item) const{
                    std::size_t h = 0;
                    for(auto iter = item.begin(), end = item.end(); iter != end; ++iter){
                        auto &a(*iter);
                        hash_combine(h, a.first);
                        hash_combine(h, a.second);
                    }
                    return h;
                }
            };
        };

        struct table_element_type{
            const actions_type *actions;
            const goto_fns_type *goto_fns;
        };

        typedef std::vector<table_element_type> action_table_type;
        typedef std::set<actions_type> actions_set_type;
        typedef std::set<goto_fns_type> goto_fns_set_type;
        struct conflict_type{
            conflict_type() : x(x_), y(y_){}
            conflict_type(const conflict_type &other) : x_(other.x_), y_(other.y_), x(x_), y(y_){}
            conflict_type(conflict_type &&other)
                : x_(std::move(other.x_)), y_(std::move(other.y_)), x(x_), y(y_){}

            conflict_type &operator =(const std::pair<action_type, action_type> &pair){
                if(pair.first < pair.second){
                    x_ = pair.first;
                    y_ = pair.second;
                }else{
                    x_ = pair.second;
                    y_ = pair.first;
                }
                return *this;
            }

            bool operator ==(const conflict_type &other) const{
                return x == other.x && y == other.y;
            }

            bool operator <(const conflict_type &other) const{
                if(x < other.x){
                    return true;
                }else if(other.x < x){
                    return false;
                }
                if(y < other.y){
                    return true;
                }
                return false;
            }

            const action_type &x, &y;

        private:
            action_type x_, y_;
        };

        typedef std::set<conflict_type> conflict_set_type;
        enum terminal_symbol_linkdir{
            nonassoc = 0,
            left,
            right
        };

        struct symbol_data_type{
            symbol_data_type() :
                linkdir(nonassoc), priority(0)
            {}

            symbol_data_type(const symbol_data_type &other) :
                linkdir(other.linkdir), priority(other.priority)
            {}

            // 結合方向
            terminal_symbol_linkdir linkdir;
            // 優先順位
            std::size_t priority;
        };

        typedef std::unordered_map<term_type, symbol_data_type> symbol_data_map_type;
        enum{
            action_error = 0,
            action_shift,
            action_reduce,
            action_acc
        };

        struct make_parsing_table_option{
            make_parsing_table_option() :
                avoid_conflict(false),
                disambiguating(false),
                put_time(false),
                put_alltime(false),
                put_log(false)
            {}

            bool avoid_conflict, disambiguating, put_time, put_alltime, put_log;
        };

        void add_terminal_symbol(const term_type &term, const symbol_data_type &symbol_data){
            terminal_symbol_map.insert(term);
            terminal_data_map[term] = symbol_data;
        }

        void add_expression(const expression_type &expression){
            expression_set.insert(expression);
        }

        lalr1_type() :
            expression_set(),
            terminal_symbol_map(),
            nonterminal_symbol_map(),
            terminal_data_map(),
            nonterminal_data_map(),
            items_set(),
            parsing_table(),
            actions_set(),
            goto_fns_set(),
            conflict_set(),
            ref_parsing_table(parsing_table)
        {}

        lalr1_type(const lalr1_type &other) :
            expression_set(other.expression_set),
            terminal_symbol_map(other.terminal_symbol_map),
            nonterminal_symbol_map(other.nonterminal_symbol_map),
            terminal_data_map(other.terminal_data_map),
            nonterminal_data_map(other.nonterminal_data_map),
            items_set(other.items_set),
            parsing_table(other.parsing_table),
            actions_set(other.actions_set),
            goto_fns_set(other.goto_fns_set),
            conflict_set(other.conflict_set),
            ref_parsing_table(parsing_table)
        {}

        lalr1_type(lalr1_type &&other) :
            expression_set(std::move(other.expression_set)),
            terminal_symbol_map(std::move(other.terminal_symbol_map)),
            nonterminal_symbol_map(std::move(other.nonterminal_symbol_map)),
            terminal_data_map(std::move(other.terminal_data_map)),
            nonterminal_data_map(std::move(other.nonterminal_data_map)),
            items_set(std::move(other.items_set)),
            parsing_table(std::move(other.parsing_table)),
            actions_set(std::move(other.actions_set)),
            goto_fns_set(std::move(other.goto_fns_set)),
            conflict_set(std::move(other.conflict_set)),
            ref_parsing_table(parsing_table)
        {}
            
        virtual ~lalr1_type(){}

        template<class IsNotTerminal, class TermToStr>
        bool make_parsing_table(
            const expression_type &start_prime,
            const term_type &dummy_term,
            const make_parsing_table_option &option,
            const IsNotTerminal &is_not_terminal,
            const TermToStr &term_to_str
        ){
            boost::timer alltime;
            alltime.restart();

            // 非終端記号のデータを収集
            {
                nonterminal_symbol_map.insert(start_prime.lhs);
                for(auto iter = expression_set.begin(), end = expression_set.end(); iter != end; ++iter){
                    auto &expression(*iter);
                    if(expression == start_prime){ continue; }
                    nonterminal_symbol_map.insert(expression.lhs);
                }
            }

            boost::timer t;

            // FIRST 集合を構築する
            t.restart();
            fset_type first_set;
            make_first_set(first_set, expression_set, nonterminal_symbol_map, is_not_terminal);
            if(option.put_time){
                std::cout << "make first-set :\n  " << t.elapsed() << "sec\n";
            }

            items_set_type slr_items_set;
            item_set_type closure_init;
            typename items_set_type::iterator first_items_iter;
            item_type i;

            //LR(0)項集合を求める
            t.restart();
            {
                item_set_type init;
                i.lhs = start_prime.lhs;
                i.pos = 0;
                i.rhs = &*start_prime.rhs.begin();
                init.insert(i);
                slr_items(slr_items_set, expression_set, terminal_symbol_map, nonterminal_symbol_map, init, is_not_terminal);
            }
            if(option.put_time){
                std::cout << "lr0 items :\n  " << t.elapsed() << "sec\n";
            }

            // LR(0) 主要項を求める
            t.restart();
            i.lhs = start_prime.lhs;
            i.rhs = &*start_prime.rhs.begin();
            i.pos = 0;
            closure_init.insert(i);
            first_items_iter = lr0_main_items(items_set, slr_items_set, start_prime.lhs);
            if(option.put_time){
                std::cout << "lr0 main items :\n  " << t.elapsed() << "sec\n";
            }

            // 行き先関数 goto を map 化する
            t.restart();
            make_goto_map(
                slr_items_set,
                items_set,
                expression_set,
                terminal_symbol_map,
                nonterminal_symbol_map,
                start_prime.lhs,
                is_not_terminal
            );
            if(option.put_time){
                std::cout << "make goto-fn map :\n  " << t.elapsed() << "sec\n";
            }

            // LR(0) 主要項集合を LALR(1) 化する
            t.restart();
            decision_lookahead(items_set, first_items_iter, expression_set, first_set, dummy_term, eos_functor_type()(), is_not_terminal);
            if(option.put_time){
                std::cout << "decision lookahead :\n  " << t.elapsed() << "sec\n";
            }

            // 状態の出力
            if(option.put_log){
                std::ofstream ofile("main_items.txt");
                if(ofile){
                    put_items(ofile, items_set, first_items_iter, term_to_str);
                }
            }

            // カーネルを閉じる
            t.restart();
            slr_items_set.clear(); // ここで mirror は無効になる
            first_items_iter = closing_kernel(items_set, first_items_iter, expression_set, first_set, is_not_terminal);
            if(option.put_time){
                std::cout << "closing kernel :\n  " << t.elapsed() << "sec\n";
            }

            // カーネルを閉じた状態の出力
            if(option.put_log){
                std::ofstream ofile("items.txt");
                if(ofile){
                    put_items(ofile, items_set, first_items_iter, term_to_str);
                }
            }

            // 解析表を構築する
            t.restart();
            make_action_table(
                items_set,
                first_items_iter,
                expression_set,
                start_prime.lhs,
                first_set,
                terminal_data_map,
                option,
                is_not_terminal,
                eps_functor_type()(),
                eos_functor_type()()
            );
            if(option.put_time){
                std::cout << "make parsing table :\n  " << t.elapsed() << "sec\n";
            }

            if(put_err_make_table(std::cout, term_to_str)){
                return false;
            }

            if(option.put_log){
                std::ofstream ofile("parsing_table.txt");
                put_parsing_table(ofile, parsing_table, term_to_str);
            }

            if(option.put_alltime){
                std::cout << "total :\n  " << alltime.elapsed() << "sec\n";
            }

            return true;
        }

    private:
        template<class IsNotTerminal>
        void make_action_table(
            const items_set_type &items_set,
            typename items_set_type::const_iterator first_items_iterator,
            const expression_set_type &expression_set,
            const term_type &first_term,
            const fset_type &first_set,
            const symbol_data_map_type &terminal_data_map,
            const make_parsing_table_option &option,
            const IsNotTerminal &is_not_terminal,
            const term_type &epsilon,
            const term_type &end_of_seq
        ){
            typedef std::unordered_map<const item_set_type*, std::size_t> state_to_index_type;
            state_to_index_type state_to_index;

            std::size_t state_num = 0;
            state_to_index[&*first_items_iterator] = state_num++;
            for(auto items_iter = items_set.begin(), items_end = items_set.end(); items_iter != items_end; ++items_iter){
                if(items_iter == first_items_iterator){ continue; }
                state_to_index[&*items_iter] = state_num++;
            }

            parsing_table.clear();
            parsing_table.resize(state_num);

            terminal_symbol_linkdir ref_nonassoc(nonassoc);
            auto rhs_priority = [&](const typename expression_type::rhs_type *rhs) -> std::pair<std::size_t, terminal_symbol_linkdir>{
                std::pair<std::size_t, terminal_symbol_linkdir> p = std::make_pair(0, ref_nonassoc);
                for(auto iter = rhs->rbegin(), end = rhs->rend(); iter != end; ++iter){
                    const term_type &term(*iter);
                    if(!is_not_terminal(expression_set, term)){
                        auto &data(terminal_data_map.find(term)->second);
                        p = std::make_pair(data.priority, data.linkdir);
                    }
                }
                return p;
            };

            auto append_conflict = [&](action_type x, action_type y) -> void{
                conflict_type conf;
                conf = std::make_pair(x, y);
                conflict_set.insert(conf);
            };

            // !!
            // g++ 4.7.0 だと slr_goto がキャプチャされない
            void (*ref_slr_goto)(
                item_set_type&,
                const expression_set_type&,
                const item_set_type&,
                const term_type&,
                const IsNotTerminal&
            ) = slr_goto;

            auto dec_act = [&](const item_set_type &i) -> const actions_type*{
                actions_type actions_i;
                for(auto item_iter = i.begin(), item_end = i.end(); item_iter != item_end; ++item_iter){
                    auto &item(*item_iter);
                    auto append_reduce = [&](const term_type &term, const action_type &act) -> bool{
                        auto ret = actions_i.insert(std::make_pair(term, act));
                        if(!ret.second){
                            auto &other_act(ret.first->second);
                            if(other_act.first == action_reduce){
                                if(option.disambiguating){
                                    if(act.item->rhs->number() < other_act.item->rhs->number()){
                                        actions_i.erase(ret.first);
                                        actions_i.insert(std::make_pair(term, act));
                                    }else if(act.item->rhs->number() == other_act.item->rhs->number()){
                                        append_conflict(other_act, act);
                                        return false;
                                    }
                                }else{
                                    append_conflict(other_act, act);
                                    return false;
                                }
                            }else{
                                if(option.avoid_conflict){
                                    // !!
                                    // MSVCで std::pair<std::size_t, terminal_symbol_linkdir> にすると error C2273 が発生
                                    std::pair<std::size_t, std::size_t> p, q;
                                    auto tag_p = act.item->rhs->tag();
                                    if(tag_p == epsilon){
                                        auto &x(terminal_data_map.find(epsilon)->second);
                                        p = std::make_pair(x.priority, x.linkdir);
                                    }else{
                                        p = rhs_priority(act.item->rhs);
                                    }
                                    auto tag_q = other_act.item->rhs->tag();
                                    if(tag_q == epsilon){
                                        auto &x(terminal_data_map.find(epsilon)->second);
                                        q = std::make_pair(x.priority, x.linkdir);
                                    }else{
                                        q = rhs_priority(other_act.item->rhs);
                                    }
                                    if(p.first > q.first || (p.first == q.first && p.second == left)){
                                        actions_i.erase(ret.first);
                                        actions_i.insert(std::make_pair(term, act));
                                    }
                                }else{
                                    append_conflict(other_act, act);
                                    return false;
                                }
                            }
                        }
                        return true;
                    };
                    action_type act;
                    act.item = &item;
                    if(item.pos < item.rhs->size()){ //shift
                        std::size_t index_j;
                        auto &a((*item.rhs)[item.pos]);
                        if(is_not_terminal(expression_set, a)){ continue; }
                        {
                            item_set_type j_;
                            ref_slr_goto(j_, expression_set, i, a, is_not_terminal);
                            index_j = state_to_index[&*items_set.find(j_)];
                        }
                        act.first = action_shift;
                        act.second = index_j;
                        auto ret = actions_i.insert(std::make_pair(a, act));
                        if(!ret.second){
                            auto &other_act(ret.first->second);
                            if(other_act.first == action_reduce){
                                std::pair<std::size_t, terminal_symbol_linkdir> p;
                                auto tag = act.item->rhs->tag();
                                if(tag == epsilon){
                                    auto &x(terminal_data_map.find(tag)->second);
                                    p = std::make_pair(x.priority, x.linkdir);
                                }else{
                                    p = rhs_priority(act.item->rhs);
                                }
                                auto &q(terminal_data_map.find(act.item->lhs)->second);
                                if(p.first > q.priority || (p.first == q.priority && p.second == left)){
                                    /* empty */
                                }else{
                                    actions_i.erase(ret.first);
                                    actions_i.insert(std::make_pair(a, act));
                                }
                            }else{
                                append_conflict(ret.first->second, act);
                            }
                        }
                    }else if(item.lhs != first_term){ //reduce
                        act.first = action_reduce;
                        act.second = 0;
                        for(auto la_iter = item.lookahead.begin(), la_end = item.lookahead.end(); la_iter != la_end; ++la_iter){
                            append_reduce(*la_iter, act);
                        }
                    }else{ //accept
                        act.first = action_acc;
                        act.second = 0;
                        append_reduce(end_of_seq, act);
                    }
                }
                auto ret = actions_set.insert(actions_i);
                return &*ret.first;
            };

            auto dec_goto = [&](const item_set_type &i) -> const goto_fns_type*{
                goto_fns_type goto_fn_map;
                for(auto exp_iter = expression_set.begin(), exp_end = expression_set.end(); exp_iter != exp_end; ++exp_iter){
                    auto &a(exp_iter->lhs);
                    std::size_t index_j;
                    {
                        item_set_type j_;
                        ref_slr_goto(j_, expression_set, i, a, is_not_terminal);
                        auto iter = items_set.find(j_);
                        if(iter == items_set.end()){ continue; }
                        index_j = state_to_index[&*iter];
                    }
                    goto_fn_map.insert(std::make_pair(a, index_j));
                }
                return &*goto_fns_set.insert(goto_fn_map).first;
            };

            std::size_t current_index = 0;
            auto &table_element(parsing_table[current_index++]);
            auto &first_items(*first_items_iterator);
            table_element.actions = dec_act(first_items);
            table_element.goto_fns = dec_goto(first_items);
            for(auto item_set_iter = items_set.begin(), item_set_end = items_set.end(); item_set_iter != item_set_end; ++item_set_iter){
                if(item_set_iter == first_items_iterator){ continue; }
                auto &table_element(parsing_table[current_index++]);
                auto &item_set(*item_set_iter);
                table_element.actions = dec_act(item_set);
                table_element.goto_fns = dec_goto(item_set);
            }
        }

        template<class IsNotTerminal>
        static void lr_goto(
            item_set_type &result,
            const term_type &x,
            const expression_set_type &expression_set,
            const fset_type &first_set,
            const IsNotTerminal &is_not_terminal
        ){
            item_set_type new_set;
            for(typename item_set_type::iterator iter = result.begin(), end = result.end(); iter != end; ++iter){
                auto &i(*iter);
                if(i.pos < i.rhs->size() && (*i.rhs)[i.pos] == x){
                    auto next_i(i);
                    ++next_i.pos;
                    new_set.insert(next_i);
                }else{
                    new_set.insert(i);
                }
            }
            lr_closure(new_set, expression_set, first_set, is_not_terminal);
            result = std::move(new_set);
        }

        template<class IsNotTerminal>
        static typename items_set_type::iterator closing_kernel(
            items_set_type &items_set,
            typename items_set_type::iterator first_items_iterator,
            const expression_set_type &expression_set,
            const fset_type &first_set,
            const IsNotTerminal &is_not_terminal
        ){
            items_set_type new_set;
            item_set_type first_items;
            for(typename items_set_type::iterator iter = items_set.begin(), end = items_set.end(); iter != end; ++iter){
                item_set_type item_set(*iter), closure_init(*iter);
                lr_closure(item_set, expression_set, first_set, is_not_terminal);
                if(first_items_iterator == iter){
                    first_items = *new_set.insert(item_set).first;
                }else{
                    new_set.insert(item_set);
                }
            }
            items_set = std::move(new_set);
            return items_set.find(first_items);
        }

        template<class IsNotTerminal>
        static void lr_closure(
            item_set_type &result,
            const expression_set_type &expression_set,
            const fset_type &first_set,
            const IsNotTerminal &is_not_terminal
        ){
            const term_type epsilon_term = eps_functor_type()();
            bool z;
            do{
                z = false;
                for(auto item_iter = result.begin(), item_end = result.end(); item_iter != item_end; ++item_iter){
                    auto &item(*item_iter);
                    if(item.pos == item.rhs->size() || !is_not_terminal(expression_set, (*item.rhs)[item.pos])){ continue; }
                    expression_type dummy_lhs((*item.rhs)[item.pos]);
                    auto &exp_b(*expression_set.find(dummy_lhs));
                    for(auto rhs_iter = exp_b.rhs.begin(), rhs_end = exp_b.rhs.end(); rhs_iter != rhs_end; ++rhs_iter){
                        item_type nitem;
                        nitem.lhs = exp_b.lhs;
                        nitem.rhs = &*rhs_iter;
                        nitem.pos = 0;
                        std::size_t i = item.pos + 1, i_len = item.rhs->size();
                        for(; i < i_len; ++i){
                            auto &beta((*item.rhs)[i]);
                            bool y = false;
                            if(!is_not_terminal(expression_set, beta)){
                                nitem.lookahead.insert(beta);
                                break;
                            }
                            auto &fset(first_set.find(beta)->second);
                            for(auto f_iter = fset.begin(), f_end = fset.end(); f_iter != f_end; ++f_iter){
                                auto &term(*f_iter);
                                if(term == epsilon_term){ y = true; }else{ nitem.lookahead.insert(term); }
                            }
                            if(!y){ break; }
                        }
                        if(i == i_len){
                            for(auto a_iter = item.lookahead.begin(), a_end = item.lookahead.end(); a_iter != a_end; ++a_iter){
                                nitem.lookahead.insert(*a_iter);
                            }
                        }
                        auto insert_result = result.insert(nitem);
                        if(!(z |= insert_result.second)){
                            const item_type &dst(*insert_result.first);
                            for(auto iter = nitem.lookahead.begin(), end = nitem.lookahead.end(); iter != end; ++iter){
                                z |= dst.lookahead.insert(*iter).second;
                            }
                        }
                    }
                }
            }while(z);
        }

        template<class IsNotTerminal>
        static void decision_lookahead(
            items_set_type &items_set,
            const typename items_set_type::iterator first_state_iterator,
            const expression_set_type &expression_set,
            const fset_type &first_set,
            const term_type &dummy_term,
            const term_type &end_of_seq_term,
            const IsNotTerminal &is_not_terminal
        ){
            first_state_iterator->begin()->lookahead.insert(end_of_seq_term);
            std::map<const item_type*, std::set<const item_type*>> propagate_map;
            for(auto items_iter = items_set.begin(), items_end = items_set.end(); items_iter != items_end; ++items_iter){
                auto &item_set(*items_iter);
                for(auto item_iter = item_set.begin(), item_end = item_set.end(); item_iter != item_end; ++item_iter){
                    auto &item(*item_iter);
                    auto &propagate_map_set(propagate_map[&item]);
                    item_set_type j;
                    {
                        auto a(item);
                        a.lookahead.clear();
                        a.lookahead.insert(dummy_term);
                        j.insert(a);
                        lr_closure(j, expression_set, first_set, is_not_terminal);
                    }
                    for(auto j_iter = j.begin(), j_end = j.end(); j_iter != j_end; ++j_iter){
                        auto &b(*j_iter);
                        if(b.pos == b.rhs->size()){ continue; }
                        auto &x((*b.rhs)[b.pos]);
                        auto &next_i(*item_set.goto_map[x]);
                        const item_type *next_b;
                        {
                            auto next_b_(b);
                            ++next_b_.pos;
                            auto ret_iter = next_i.find(next_b_);
                            next_b = &*ret_iter;
                        }
                        const item_type &c_next_b(*next_b);
                        for(auto la_iter = b.lookahead.begin(), la_end = b.lookahead.end(); la_iter != la_end; ++la_iter){
                            auto &la(*la_iter);
                            if(la != dummy_term){
                                c_next_b.lookahead.insert(la);
                            }else{
                                propagate_map_set.insert(&c_next_b);
                            }
                        }
                    }
                }
            }
            bool z;
            do{
                z = false;
                for(
                    auto propagate_iter = propagate_map.begin(), propagate_end = propagate_map.end();
                    propagate_iter != propagate_end;
                    ++propagate_iter
                ){
                    auto &propagate_pair(*propagate_iter);
                    auto &propagate_src(*propagate_pair.first);
                    auto &propagate_dst_set(propagate_pair.second);
                    for(auto iter = propagate_dst_set.begin(), end = propagate_dst_set.end(); iter != end; ++iter){
                        auto &dst_item(**iter);
                        std::size_t prev_size = dst_item.lookahead.size();
                        dst_item.lookahead.insert(propagate_src.lookahead.begin(), propagate_src.lookahead.end());
                        if(dst_item.lookahead.size() != prev_size){ z = true; }
                    }
                }
            }while(z);
        }

        template<class IsNotTerminal>
        static void make_goto_map(
            const items_set_type &items_set,
            const items_set_type &main_items_set,
            const expression_set_type &expression_set,
            const term_set_type &terminal_symbol_map,
            const term_set_type &nonterminal_symbol_map,
            const term_type &first_term,
            const IsNotTerminal &is_not_terminal
        ){
            for(auto item_set_iter = items_set.begin(), item_set_end = items_set.end(); item_set_iter != item_set_end; ++item_set_iter){
                auto &item_set(*item_set_iter);
                // !!
                // MSVC10 だと slr_goto がキャプチャされない
                void (*ref_slr_goto)(
                    item_set_type&,
                    const expression_set_type&,
                    const item_set_type&,
                    const term_type&,
                    const IsNotTerminal&
                ) = slr_goto;
                // !!
                // gcc 4.7.0 だと main_items がキャプチャされない
                auto ref_main_items(main_items);
                auto fn = [&](const term_set_type &symbol_set){
                    for(auto term_iter = symbol_set.begin(), term_end = symbol_set.end(); term_iter != term_end; ++term_iter){
                        auto &x(*term_iter);
                        item_set_type goto_ix, goto_ix_;
                        ref_slr_goto(goto_ix, expression_set, item_set, x, is_not_terminal);
                        if(goto_ix.empty()){ continue; }
                        ref_main_items(goto_ix_, goto_ix, first_term);
                        item_set.mirror->goto_map[x] = &*main_items_set.find(goto_ix_);
                    }
                };
                fn(terminal_symbol_map);
                fn(nonterminal_symbol_map);
            }
        }

        static bool main_items(item_set_type &result, const item_set_type &input, const term_type first_term){
            bool b = false;
            for(auto iter = input.begin(), end = input.end(); iter != end; ++iter){
                auto &item(*iter);
                bool c;
                if((c = item.lhs == first_term) || item.pos > 0){
                    result.insert(item);
                    b = c && item.pos == 0;
                }
            }
            return b;
        }

        static typename items_set_type::iterator lr0_main_items(
            items_set_type &result,
            const items_set_type &i_set,
            const term_type first_term
        ){
            typename items_set_type::iterator r;
            for(
                typename items_set_type::iterator item_set_iter = i_set.begin(), item_set_end = i_set.end();
                item_set_iter != item_set_end;
                ++item_set_iter
            ){
                auto &item_set(*item_set_iter);
                item_set_type n_item_set;
                bool b = main_items(n_item_set, item_set, first_term);
                auto q = result.insert(n_item_set);
                q.first->mirror = &item_set;
                item_set.mirror = &*q.first;
                if(b){ r = q.first; }
            }
            return r;
        }

        template<class IsNotTerminal>
        static void slr_closure(
            item_set_type &result,
            const expression_set_type &expression_set,
            const item_set_type &i_set,
            const IsNotTerminal &is_not_terminal
        ){
            result.insert(i_set.begin(), i_set.end());
            bool z;
            do{
                z = false;
                for(auto a_iter = result.begin(), a_end = result.end(); a_iter != a_end; ++a_iter){
                    auto &item_a(*a_iter);
                    if(item_a.pos == item_a.rhs->size()){ continue; }
                    auto &b((*item_a.rhs)[item_a.pos]);
                    if(!is_not_terminal(expression_set, b)){ continue; }
                    auto &exp_b(*expression_set.find(b));
                    item_type nitem;
                    nitem.lhs = b;
                    for(auto rhs_iter = exp_b.rhs.begin(), rhs_end = exp_b.rhs.end(); rhs_iter != rhs_end; ++rhs_iter){
                        nitem.pos = 0;
                        nitem.rhs = &*rhs_iter;
                        z |= result.insert(nitem).second;
                    }
                }
            }while(z);
        }

        template<class IsNotTerminal>
        static void slr_goto(
            item_set_type &result,
            const expression_set_type &expression_set,
            const item_set_type &i_set,
            const term_type &x,
            const IsNotTerminal &is_not_terminal
        ){
            for(auto iter = i_set.begin(), end = i_set.end(); iter != end; ++iter){
                auto item(*iter);
                if(item.pos == item.rhs->size() || (*item.rhs)[item.pos] != x){ continue; }
                ++item.pos;
                result.insert(item);
            }
            item_set_type new_set;
            slr_closure(new_set, expression_set, result, is_not_terminal);
            result = new_set;
        }

        template<class IsNotTerminal>
        static typename items_set_type::iterator slr_items(
            items_set_type &result,
            const expression_set_type &expression_set,
            const term_set_type &terminal_symbol_map,
            const term_set_type &nonterminal_symbol_map,
            const item_set_type &i_set,
            const IsNotTerminal &is_not_terminal
        ){
            typename items_set_type::iterator ret;
            {
                item_set_type a;
                slr_closure(a, expression_set, i_set, is_not_terminal);
                item_set_type first_item_set;
                first_item_set.insert(a.begin(), a.end());
                ret = result.insert(first_item_set).first;
            }
            bool z;
            do{
                z = false;
                for(auto item_set_iter = result.begin(), item_set_end = result.end(); item_set_iter != item_set_end; ++item_set_iter){
                    auto &item_set(*item_set_iter);
                    // !!
                    // MSVC10だとslr_gotoがキャプチャされない
                    void (*ref_slr_goto)(
                        item_set_type&,
                        const expression_set_type&,
                        const item_set_type&,
                        const term_type&,
                        const IsNotTerminal&
                    ) = slr_goto;
                    auto fn = [&](const term_set_type &symbol_set){
                        for(auto x_iter = symbol_set.begin(), x_end = symbol_set.end(); x_iter != x_end; ++x_iter){
                            auto &x(*x_iter);
                            item_set_type n_item_set;
                            ref_slr_goto(n_item_set, expression_set, item_set, x, is_not_terminal);
                            if(!n_item_set.empty()){
                                z |= result.insert(n_item_set).second;
                            }
                        }
                    };
                    fn(terminal_symbol_map);
                    fn(nonterminal_symbol_map);
                }
            }while(z);
            return ret;
        }

        template<class IsNotTerminal>
        static void make_first_set(
            fset_type &first_set,
            const expression_set_type &expression_set,
            const term_set_type &nonterminal_symbol_map,
            const IsNotTerminal &is_not_terminal
        ){
            typename expression_type::rhs_type rhs_epsilon;
            term_type term_epsilon = eps_functor_type()();
            bool z;
            do{
                z = false;
                for(auto x_iter = nonterminal_symbol_map.begin(), x_end = nonterminal_symbol_map.end(); x_iter != x_end; ++x_iter){
                    auto &x(*x_iter);
                    {
                        expression_type dummy_lhs(x);
                        const typename expression_type::rhs_set_type &rhs(expression_set.find(dummy_lhs)->rhs);
                        if(rhs.find(rhs_epsilon) != rhs.end() && first_set[x].find(term_epsilon) == first_set[x].end()){
                            first_set[x].insert(term_epsilon);
                            z = true;
                        }
                    }
                    auto expression_set_find_result = expression_set.find(x);
                    for(auto rhs_iter = expression_set_find_result->rhs.begin(), rhs_end = expression_set_find_result->rhs.end(); rhs_iter != rhs_end; ++rhs_iter){
                        auto &rhs(*rhs_iter);
                        for(auto y_iter = rhs.begin(), y_end = rhs.end(); y_iter != y_end; ++y_iter){
                            auto &y(*y_iter);
                            if(x == y){ continue; }
                            if(is_not_terminal(expression_set, y)){
                                typename fset_type::mapped_type &first_set_x(first_set[x]), &first_set_y(first_set[y]);
                                for(
                                    auto first_set_y_iter = first_set_y.begin(), first_set_y_end = first_set_y.end();
                                    first_set_y_iter != first_set_y_end;
                                    ++first_set_y_iter
                                ){
                                    auto &first_set_y(*first_set_y_iter);
                                    z = z | (first_set_x.find(first_set_y) == first_set_x.end());
                                    if(z){
                                        first_set_x.insert(first_set_y);
                                    }
                                }
                                auto &y_rhs(expression_set.find(y)->rhs);
                                if(first_set_y.find(term_epsilon) == first_set_y.end()){ break; }
                            }else{
                                z |= first_set[x].insert(y).second;
                                break;
                            }
                        }
                    }
                }
            }while(z);
        }

        template<class TermToStr>
        void put_items(
            std::ostream &stream,
            const items_set_type &items_set,
            const typename items_set_type::const_iterator first_items_iter,
            const TermToStr &term_to_str
        ){
            const char epsilon_str[] = "(epsilon)";
            auto put_delim = [&](std::size_t m, std::size_t len)->void{
                for(std::size_t n = 0; n < len - m; ++n){
                    stream << static_cast<char>(' ');
                }
            };
            auto fn = [&](std::size_t i, const item_set_type &item_set) -> void{
                std::size_t lhs_length = 0, rhs_length = 0;
                stream << "--------I_" << i << "\n";
                for(auto item_iter = item_set.begin(), item_end = item_set.end(); item_iter != item_end; ++item_iter){
                    auto &item(*item_iter);
                    auto lhs(term_to_str(item.lhs));
                    if(lhs.size() + 1 > lhs_length){ lhs_length = lhs.size() + 1; }
                    std::size_t n_rhs_length = 0;
                    if(item.rhs->empty()){
                        n_rhs_length = sizeof(epsilon_str) / sizeof(epsilon_str[0]);
                    }else{
                        for(auto rhs_iter = item.rhs->begin(), rhs_end = item.rhs->end(); rhs_iter != rhs_end; ++rhs_iter){
                            auto rhs(term_to_str(*rhs_iter));
                            n_rhs_length += rhs.size() + 1;
                        }
                    }
                    if(n_rhs_length > rhs_length){ rhs_length = n_rhs_length; }
                }
                for(auto item_iter = item_set.begin(), item_end = item_set.end(); item_iter != item_end; ++item_iter){
                    auto &item(*item_iter);
                    auto lhs(term_to_str(item.lhs));
                    stream << lhs;
                    put_delim(lhs.size() + 1, lhs_length);
                    stream << " = ";
                    if(item.rhs->empty()){
                        stream << static_cast<char>('.') << epsilon_str;
                        put_delim(sizeof(epsilon_str) / sizeof(epsilon_str[0]) - 1, rhs_length);
                    }else{
                        std::size_t n_rhs_length = 0, count = 0;
                        for(auto rhs_iter = item.rhs->begin(), rhs_end = item.rhs->end(); rhs_iter != rhs_end; ++rhs_iter, ++count){
                            auto rhs(term_to_str(*rhs_iter));
                            n_rhs_length += rhs.size() + 1;
                            if(count == item.pos){
                                stream << static_cast<char>('.') << rhs << static_cast<char>(' ');
                            }else{
                                auto n_rhs_iter(rhs_iter + 1);
                                if(n_rhs_iter == rhs_end && item.pos == count + 1){
                                    stream << rhs << ". ";
                                }else{
                                    stream << rhs << static_cast<char>(' ');
                                }
                            }
                        }
                        put_delim(n_rhs_length, rhs_length);
                    }
                    stream << ":";
                    for(auto la_iter = item.lookahead.begin(), la_end = item.lookahead.end(); la_iter != la_end; ++la_iter){
                        auto la(term_to_str(*la_iter));
                        if(la.size() == 0){
                            stream << " " << epsilon_str;
                        }else{
                            stream << static_cast<char>(' ') << la;
                        }
                    }
                    stream << "\n";
                }
            };
            std::size_t i = 0;
            fn(i++, *first_items_iter);
            stream << "\n";
            for(auto item_set_iter = items_set.begin(), item_set_end = items_set.end(); item_set_iter != item_set_end; ++item_set_iter){
                if(item_set_iter == first_items_iter){ continue; }
                auto &item_set(*item_set_iter);
                fn(i, item_set);
                stream << "\n";
                ++i;
            }
        }

        template<class TermToStr>
        bool put_err_make_table(
            std::ostream &stream,
            const TermToStr &term_to_str
        ){
            bool z = false;
            for(auto conf_iter = conflict_set.begin(), conf_end = conflict_set.end(); conf_iter != conf_end; ++conf_iter){
                z = true;
                auto &conf(*conf_iter);
                std::size_t pattern_id[] = { conf.x.first, conf.y.first };
                const char *pattern_str[2];
                for(int i = 0; i < 2; ++i){
                    switch(pattern_id[i]){
                    case action_shift:
                        pattern_str[i] = "sft";
                        break;

                    case action_reduce:
                        pattern_str[i] = "rdc";
                        break;

                    case action_acc:
                        pattern_str[i] = "acc";
                        break;

                    default:
                        pattern_str[i] = "err";
                        break;
                    }
                }
                auto &p_item(*conf.x.item);
                auto &c_item(*conf.y.item);
                std::size_t term_count;
                stream << pattern_str[0] << "/" << pattern_str[1] << " conflict : ";
                stream << term_to_str(p_item.lhs) << " = ";
                term_count = 0;
                for(auto iter = p_item.rhs->begin(), end = p_item.rhs->end(); iter != end; ++iter, ++term_count){
                    if(term_count == p_item.pos){ std::cout << "."; }
                    std::cout << term_to_str(*iter) << " ";
                }
                if(term_count == p_item.pos){ std::cout << ". "; }
                stream << "vs ";
                stream << term_to_str(c_item.lhs) << " = ";
                term_count = 0;
                for(auto iter = c_item.rhs->begin(), end = c_item.rhs->end(); iter != end; ++iter, ++term_count){
                    if(term_count == c_item.pos){ std::cout << "."; }
                    stream << term_to_str(*iter) << " ";
                }
                if(term_count == c_item.pos){ std::cout << ". "; }
                stream << "\n";
            }
            return z;
        }

        template<class TermToStr>
        void put_parsing_table(std::ostream &stream, const action_table_type &table, const TermToStr &term_to_str){
            std::size_t i = 0;
            for(auto table_element_iter = table.begin(), table_element_end = table.end(); table_element_iter != table_element_end; ++table_element_iter, ++i){
                stream << "--------I_" << i << "\n";
                auto &table_element(*table_element_iter);
                auto &actions(*table_element.actions);
                auto &goto_fns(*table_element.goto_fns);
                std::size_t max_term_length = 0;
                for(auto action_iter = actions.begin(), action_end = actions.end(); action_iter != action_end; ++action_iter){
                    auto term(term_to_str(action_iter->first));
                    if(max_term_length < term.size()){ max_term_length = term.size(); }
                }
                for(auto goto_fn_iter = goto_fns.begin(), goto_fn_end = goto_fns.end(); goto_fn_iter != goto_fn_end; ++goto_fn_iter){
                    auto term(term_to_str(goto_fn_iter->first));
                    if(max_term_length < term.size()){ max_term_length = term.size(); }
                }
                auto put_delim = [&](std::size_t m) -> void{
                    for(std::size_t n = 0; n < max_term_length + 1 - m; ++n){
                        stream << static_cast<char>(' ');
                    }
                };
                for(auto action_iter = actions.begin(), action_end = actions.end(); action_iter != action_end; ++action_iter){
                    auto &term_action(*action_iter);
                    auto term(term_to_str(term_action.first));
                    auto &action(term_action.second);
                    stream << term;
                    put_delim(term.size());
                    stream << ": ";
                    switch(action.first){
                    case action_shift:
                        stream << "shift " << action.second;
                        break;

                    case action_reduce:
                        stream << "reduce " << term_to_str(action.item->lhs);
                        break;

                    case action_acc:
                        stream << "accept!!";
                        break;
                    }
                    stream << "\n";
                }
                for(auto goto_fn_iter = goto_fns.begin(), goto_fn_end = goto_fns.end(); goto_fn_iter != goto_fn_end; ++goto_fn_iter){
                    auto term(term_to_str(goto_fn_iter->first));
                    stream << term;
                    put_delim(term.size());
                    stream <<  ": goto " << goto_fn_iter->second << "\n";
                }
                stream << "\n";
            }
        }
        
        expression_set_type   expression_set;
        term_set_type         terminal_symbol_map,
                              nonterminal_symbol_map;
        symbol_data_map_type  terminal_data_map,
                              nonterminal_data_map;
        items_set_type        items_set;
        action_table_type     parsing_table;
        actions_set_type      actions_set;
        goto_fns_set_type     goto_fns_set;
        conflict_set_type     conflict_set;

    public:
        const action_table_type &ref_parsing_table;
    };
}

#endif // KP19PP_HPP_
