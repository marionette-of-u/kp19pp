#ifndef KP19PP_SCANNER_HPP_
#define KP19PP_SCANNER_HPP_

#include <string>
#include <vector>
#include <forward_list>
#include <utility>
#include "kp19pp.hpp"
#include "common.hpp"

namespace kp19pp{
    namespace scanner{
        typedef int term_type;
        struct epsilon_functor{
            term_type operator ()() const;
        };

        struct end_of_seq_functor{
            term_type operator ()() const;
        };

        struct is_not_terminal_functor{
            template<class UnusedType>
            bool operator ()(const UnusedType&, term_type) const;
        };

        extern const scanner_string_type dummy_string;

        class scanner_type;
        struct semantic_type{
            typedef basic_token_type<string_iter_pair_type, term_type> token_type;
            typedef std::vector<token_type> value_type;
            typedef token_type (*action_type)(const value_type&, scanner_type&);
            action_type action;
            semantic_type();
            semantic_type(const semantic_type &other);
            semantic_type(semantic_type &&other);
            semantic_type &operator =(const semantic_type &other);
            token_type operator ()(const value_type &value, scanner_type &data) const;
            static token_type eat(const value_type &value, scanner_type &data);
        };

        class scanner_type : public lalr1_type<
            string_iter_pair_type,
            term_type,
            semantic_type,
            end_of_seq_functor,
            epsilon_functor
        >{
        private:
            typedef lalr1_type<
                string_iter_pair_type,
                term_type,
                semantic_type,
                end_of_seq_functor,
                epsilon_functor
            > base_type;

        public:
            enum class rhs_place{
                n,
                semantic_action,
                rhs_seq_head,
                rhs_seq_rest,
                union_seq,
                extended
            };

        public:
            enum analysis_phase_enum{
                phase_token,
                phase_grammar,
                phase_end
            } analysis_phase;
            
            enum token_order_enum{
                order_ascending,
                order_descending
            } token_order;

            struct symbol_type{
                symbol_type();
                symbol_type(const token_type &token);
                symbol_type(const symbol_type &other);
                bool operator ==(const symbol_type &other) const;
                bool operator <(const symbol_type &other) const;
                token_type value;
                struct hash{
                    std::size_t operator ()(const symbol_type &item) const;
                };
            };

            struct terminal_symbol_data_type{
                terminal_symbol_data_type();
                terminal_symbol_data_type(const terminal_symbol_data_type &other);
                token_type type;
                terminal_symbol_linkdir linkdir;
                std::size_t             priority;
            };

            struct nonterminal_symbol_data_type{
                nonterminal_symbol_data_type();
                nonterminal_symbol_data_type(const nonterminal_symbol_data_type &other);
                nonterminal_symbol_data_type(nonterminal_symbol_data_type &&other);

                class rhs_type : public std::vector<std::pair<symbol_type, symbol_type>>{
                private:
                    typedef std::vector<std::pair<symbol_type, symbol_type>> base_type;

                public:
                    rhs_type();
                    rhs_type(const rhs_type &other);
                    rhs_type(rhs_type &&other);
                    bool operator ==(const rhs_type &other) const;
                    void clear();
                    token_type semantic_action, tag;
                    std::size_t number;
                    struct arg_data_type{
                        symbol_type         symbol;
                        token_type          number;
                        std::size_t         src_index;
                        const token_type    mutable *type_cache;
                    };

                    typedef std::map<
                        std::size_t,
                        arg_data_type
                    > argindex_to_symbol_map_type;

                    argindex_to_symbol_map_type argindex_to_symbol_map;
                    int                         argindex_max;

                    struct hash{
                        std::size_t operator ()(const rhs_type &item) const;
                    };
                };
                
                typedef std::unordered_set<
                    rhs_type,
                    rhs_type::hash
                > rhs_set_type;

                token_type      default_semantic_action, type;
                rhs_set_type    rhs;
            };

            using terminal_symbol_map_type = std::unordered_map<
                symbol_type,
                terminal_symbol_data_type,
                symbol_type::hash
            >;
            
            using nonterminal_symbol_map_type = std::unordered_map<
                symbol_type,
                nonterminal_symbol_data_type,
                symbol_type::hash
            >;
            
            using undefined_nonterminal_symbol_set_type = std::unordered_set<
                symbol_type,
                symbol_type::hash
            >;

            using term_to_token_map_type = std::unordered_map<
                term_type,
                token_type
            >;

            using number_to_token_map_type = std::unordered_map<
                std::size_t,
                token_type
            >;

            using declared_extended_set = std::set<symbol_type>;

            using token_seq_type = std::vector<token_type>;

        public:
            scanner_type();
            void scan(std::istream &in);
            void make_target();
            token_type add_extended_rule_name(const token_type&);
            token_type add_extended_rule_name(const token_type&, std::size_t);
            term_type next_terminal_symbol_id();
            term_type next_nonterminal_symbol_id();
            std::size_t next_rhs_number();
            void clear_rhs_number();
            std::size_t next_rhs_arg_number();
            void inc_current_rhs_arg_number();
            void clear_current_rhs_arg_number();
            bool get_scanned_first_nonterminal_symbol() const;
            void set_scanned_first_nonterminal_symbol();
            bool set_scanned_extended_rule(const token_type &token);
            nonterminal_symbol_data_type &make_nonterminal_symbol(
                token_type &token,
                const token_type &nonterminal_type
            );

            void inc_union_num();
            std::size_t clear_union_num();
            nonterminal_symbol_data_type::rhs_type &current_rhs();
            nonterminal_symbol_data_type::rhs_type &front_rhs();
            void push_rhs(rhs_place);
            void pop_rhs();
            void clear_extended_data();

        private:
            static void define_grammar(scanner_type &scanner);
            void check_undefined_nonterminal_symbol();
            void check_linked_nonterminal_symbol();
            void normalize_token_order();
            void collect_token();
            void caching_arg_type() const;
            void augment();
            template<class PutFn, class ErrorFn>
            bool parse(const PutFn &put_fn, const ErrorFn &error_fn);

        public:
            value_type                              default_semantic_action;
            token_seq_type                          token_seq;
            string_iter_pair_type                   namespace_token,
                                                    namespace_grammar;
            token_type                              first_semantic_action;
            std::size_t                             current_priority,
                                                    current_token_number;
            std::vector<terminal_symbol_data_type*> current_terminal_symbol_seq;
            terminal_symbol_map_type                terminal_symbol_map;
            nonterminal_symbol_map_type             nonterminal_symbol_map;
            undefined_nonterminal_symbol_set_type   undefined_nonterminal_symbol_set;
            term_to_token_map_type                  term_to_token_map;
            nonterminal_symbol_map_type::iterator   current_nonterminal_symbol_iter;
            symbol_type                             first_nonterminal_symbol;
            number_to_token_map_type                number_to_token_map;
            bool                                    external_token;

            token_type
                current_extended_semantic_action,
                current_extended_decl,
                current_extended_type;
            rhs_place rhs_place_state = rhs_place::n;

        private:
            scanner_string_type string;
            term_type           current_terminal_symbol_id,
                                current_nonterminal_symbol_id;
            std::size_t         current_rhs_number,
                                current_rhs_arg_number;
            bool                scanned_first_nonterminal_symbol;

            std::size_t union_num = 0;
            std::vector<nonterminal_symbol_data_type::rhs_type> current_rhs_stack;

            declared_extended_set scanned_declared_extended_set;
            std::forward_list<scanner_string_type> extended_storage;
        };

#define KP19PP_SCANNER_DECL_TERMINAL_SYMBOLS() \
    DECL(identifier) \
    DECL(value) \
    DECL(comma) \
    DECL(dot) \
    DECL(question) \
    DECL(plus) \
    DECL(asterisk) \
    DECL(ampersand) \
    DECL(double_colon) \
    DECL(semicolon) \
    DECL(l_square_bracket) \
    DECL(r_square_bracket) \
    DECL(l_curly_bracket) \
    DECL(r_curly_bracket) \
    DECL(l_bracket) \
    DECL(r_bracket) \
    DECL(l_round_pare) \
    DECL(r_round_pare) \
    DECL(symbol_or) \
    DECL(symbol_colon) \
    DECL(equal) \
    DECL(str)

        enum token{
#define DECL(name) token_ ## name ,
            token_0,
            KP19PP_SCANNER_DECL_TERMINAL_SYMBOLS()
#undef DECL
        };

        namespace terminal_symbol{
#define DECL(name) \
    extern scanner_type::term_type name;
            KP19PP_SCANNER_DECL_TERMINAL_SYMBOLS()
#undef DECL
        }
    }
}

#endif // KP19PP_SCANNER_HPP_
