#include "indent.hpp"

namespace kp19pp{
    indent_type::indent_type(commandline_options_type::indent_enum indent_kind_) :
        indent_kind(indent_kind_)
    {}

    std::ostream &operator <<(std::ostream &ostream, const indent_type &i){
        switch(i.indent_kind){
        case commandline_options_type::indent_space:
        case commandline_options_type::indent_space4:
            ostream << "    ";
            break;

        case commandline_options_type::indent_space8:
            ostream << "        ";

        case commandline_options_type::indent_tab:
            ostream << "\t";
            break;
        }
        return ostream;
    }
}

