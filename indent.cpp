#include "indent.hpp"

namespace kp19pp{
    indent_type::indent_type(commandline_options_type::indent_enum indent_kind_, int n_) :
        n(n_)
    {
        switch(indent_kind_){
        case commandline_options_type::indent_space2:
            ptr = "  ";
            break;

        case commandline_options_type::indent_space:
        case commandline_options_type::indent_space4:
            ptr = "    ";
            break;

        case commandline_options_type::indent_space8:
            ptr = "        ";
            break;

        case commandline_options_type::indent_tab:
            ptr = "\t";
            break;
        }
    }

    std::ostream &operator <<(std::ostream &ostream, const indent_type &i){
        for(int count = 0; count < i.n; ++count){
            ostream << i.ptr;
        }
        return ostream;
    }
}

