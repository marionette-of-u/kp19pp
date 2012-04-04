#ifndef KP19PP_TARGET_HPP_
#define KP19PP_TARGET_HPP_

#include "kp19pp.hpp"
#include "common.hpp"
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

        class target_type : public lalr1_type<
            unused_type,
            term_type,
            string_iter_pair_type,
            end_of_seq_functor,
            epsilon_functor
        >{
        private:
            typedef lalr1_type<
                unused_type,
                term_type,
                string_iter_pair_type,
                end_of_seq_functor,
                epsilon_functor
            > base_type;

        public:
            target_type();
            bool make_parsing_table(scanner::scanner_type &scanner);

        private:
            target_type(const target_type &other);
            target_type(target_type &&other);
        };
    }
}

#endif // KP19PP_TARGET_HPP_
