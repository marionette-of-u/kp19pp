#include "exception.hpp"
#include "common.hpp"

namespace kp19pp{
    exception::exception(const std::string &str, std::size_t char_num_, std::size_t line_num_) :
        std::runtime_error(str),
        char_num(0),
        line_num(0)
    {}

    exception::exception(const exception &other) :
        std::runtime_error(other),
        char_num(other.char_num),
        line_num(other.line_num)
    {}

    exception::exception() :
        std::runtime_error("undefined exception."),
        char_num(0),
        line_num(0)
    {}

    exception::~exception() throw(){}

    exception_seq::exception_seq(const std::string &str) :
        std::runtime_error(str)
    {}

    exception_seq::exception_seq(const exception_seq &other) :
        std::runtime_error(other),
        seq(other.seq)
    {}

    exception_seq::exception_seq(exception_seq &&other) :
        std::runtime_error(other),
        seq(std::move(other.seq))
    {}

    exception_seq::exception_seq() :
        std::runtime_error("undefined exception.")
    {}

    exception_seq::~exception_seq() throw(){}
}
