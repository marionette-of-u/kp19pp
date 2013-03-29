#ifndef KP19PP_TARGET_HPP_
#define KP19PP_TARGET_HPP_

#include <fstream>
#include <iostream>
#include "kp19pp.hpp"
#include "common.hpp"
#include "commandline_options.hpp"
#include "scanner.hpp"

namespace kp19pp{
    namespace target{
        typedef int term_type;
        struct epsilon_functor{
            term_type operator ()() const;
        };

        struct end_of_seq_functor{
            term_type operator ()() const;
        };

        struct unused_type{};
        struct semantic_type{
            semantic_type();
            semantic_type(const semantic_type &other);
            semantic_type(semantic_type &&other);
            const string_iter_pair_type *action, *type;
            const scanner::scanner_type::nonterminal_symbol_data_type::rhs_type::argindex_to_symbol_map_type *argindex_to_symbol_map;
        };

        class target_type : public lalr1_type<
            unused_type,
            term_type,
            semantic_type,
            end_of_seq_functor,
            epsilon_functor
        >{
        private:
            typedef lalr1_type<
                unused_type,
                term_type,
                semantic_type,
                end_of_seq_functor,
                epsilon_functor
            > base_type;

        public:
            target_type();
            bool make_parsing_table(
                const scanner::scanner_type &scanner,
                const commandline_options_type &commandline_options
            );

        private:
            target_type(const target_type &other);
            target_type(target_type &&other);
            void generate_cpp(
                std::ostream &ostream,
                const commandline_options_type &commandline_options,
                const scanner::scanner_type &scanner
            );

            void generate_vimscript(
                std::ostream &ostream,
                const commandline_options_type &commandline_options,
                const scanner::scanner_type &scanner
            );
        };
    }
}

#endif // KP19PP_TARGET_HPP_
