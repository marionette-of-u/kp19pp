#ifndef KP19PP_INDENT_HPP_
#define KP19PP_INDENT_HPP_

#include <iostream>
#include "commandline_options.hpp"

namespace kp19pp{
    struct indent_type{
        indent_type(commandline_options_type::indent_enum indent_kind_, int n_);
        const commandline_options_type::indent_enum indent_kind;
        const int n;
    };

    std::ostream &operator <<(std::ostream &ostream, const indent_type &i);
}

#endif // KP19PP_INDENT_HPP_
