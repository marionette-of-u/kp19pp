#include "target.hpp"

namespace kp19pp{
    namespace target{
        term_type epsilon_functor::operator ()() const{
            return 0;
        }

        term_type end_of_seq_functor::operator ()() const{
            return (std::numeric_limits<int>::max)();
        }

        bool target_type::make_parsing_table(scanner::scanner_type &scanner){
            ;
            return true;
        }

        target_type::target_type() : base_type(){}
        target_type::target_type(const target_type &other) : base_type(other){}
        target_type::target_type(target_type &&other) : base_type(other){}
    }
}

