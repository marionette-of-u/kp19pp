#ifndef KP19PP_SCANNER_HPP_
#define KP19PP_SCANNER_HPP_

#include <string>
#include <vector>
#include <utility>
#include <boost/range.hpp>
#include "kp19pp.hpp"

namespace kp19pp{
    namespace scanner{
        typedef int term_type;
        struct epsilon_functor{
            term_type operator ()() const;
        };

        struct end_of_seq_functor{
            term_type operator ()() const;
        };

        typedef std::vector<char> scanner_string_type;
        class string_iter_pair_type : public boost::iterator_range<scanner_string_type::const_iterator>{
        private:
            typedef boost::iterator_range<scanner_string_type::const_iterator> base_type;

        public:
            string_iter_pair_type();
            string_iter_pair_type(const string_iter_pair_type &other);
            string_iter_pair_type(string_iter_pair_type &&other);
            string_iter_pair_type(scanner_string_type::const_iterator first, scanner_string_type::const_iterator last);
            string_iter_pair_type &operator =(const string_iter_pair_type &other);
            string_iter_pair_type &operator =(std::pair<scanner_string_type::const_iterator, scanner_string_type::const_iterator>);
            bool operator ==(const string_iter_pair_type &other) const;

        private:
            static scanner_string_type &dummy_strage();

        public:
            struct hash{
                std::size_t operator ()(const string_iter_pair_type &item) const;
            };
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
                symbol_type(const symbol_type &other);
                bool operator ==(const symbol_type &other) const;
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
                std::size_t priority;
            };

            struct nonterminal_symbol_data_type{
                nonterminal_symbol_data_type();
                nonterminal_symbol_data_type(const nonterminal_symbol_data_type &other);
                token_type type;

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
                    typedef std::map<int, std::pair<symbol_type, token_type>> argindex_to_symbol_map_type;
                    argindex_to_symbol_map_type argindex_to_symbol_map;
                    int argindex_max;
                    struct hash{
                        std::size_t operator ()(const rhs_type &item) const;
                    };
                };
                
                std::unordered_set<rhs_type, rhs_type::hash> rhs;
            };

            typedef std::unordered_map<
                symbol_type,
                terminal_symbol_data_type,
                symbol_type::hash
            > terminal_symbol_map_type;
            
            typedef std::unordered_map<
                symbol_type,
                nonterminal_symbol_data_type,
                symbol_type::hash
            > nonterminal_symbol_map_type;
            
            typedef std::unordered_set<
                symbol_type,
                symbol_type::hash
            > undefined_nonterminal_symbol_set_type;

            typedef std::vector<token_type> token_seq_type;

        public:
            scanner_type();
            void scan(std::istream &in);
            term_type next_terminal_symbol_id();
            term_type next_nonterminal_symbol_id();
            std::size_t next_rhs_number();
            void clear_rhs_number();

        private:
            static void define_grammar();
            void check_undefined_nonterminal_symbol();
            void normalize_token_order();
            template<class PutFn, class ErrorFn>
            bool parse(const PutFn &put_fn, const ErrorFn &error_fn);

        public:
            token_seq_type                              token_seq;
            scanner_string_type                         start_prime;
            string_iter_pair_type                       namespace_token,
                                                        namespace_grammar;
            std::size_t                                 current_priority;
            std::vector<terminal_symbol_data_type*>     current_terminal_symbol_seq;
            nonterminal_symbol_data_type::rhs_type      current_rhs;
            terminal_symbol_map_type                    terminal_symbol_map;
            nonterminal_symbol_map_type                 nonterminal_symbol_map;
            undefined_nonterminal_symbol_set_type       undefined_nonterminal_symbol_set;
            nonterminal_symbol_map_type::iterator       current_nonterminal_symbol_iter;

        private:
            scanner_string_type string;
            term_type           current_terminal_symbol_id,
                                current_nonterminal_symbol_id;
            std::size_t         current_rhs_number;
        };

        extern scanner_type scanner;
    }
}

#endif // KP19PP_SCANNER_HPP_
