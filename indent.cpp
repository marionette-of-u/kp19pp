#include "indent.hpp"

namespace kp19pp{
    indent_type::indent_type(commandline_options_type::indent_enum indent_kind_, int n_) :
        indent_kind(indent_kind_), n(n_)
    {}

    std::ostream &operator <<(std::ostream &ostream, const indent_type &i){
        const char *ptr = nullptr;
        switch(i.indent_kind){
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
        for(int count = 0; count < i.n; ++count){
            ostream << ptr;
        }
        return ostream;
    }
}

