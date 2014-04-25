#ifndef KP19PP_EXCEPTION_HPP_
#define KP19PP_EXCEPTION_HPP_

#include <vector>
#include <string>
#include <stdexcept>

namespace kp19pp{
    struct exception : public std::runtime_error{
        exception(const std::string &str, std::size_t char_num_, std::size_t line_num_);
        exception(const exception &other);
        exception();
        virtual ~exception() throw();
        std::size_t char_num, line_num;
    };

    struct exception_seq : public std::runtime_error{
        exception_seq(const std::string &str);
        exception_seq(const exception_seq &other);
        exception_seq(exception_seq &&other);
        exception_seq();
        virtual ~exception_seq() throw();
        std::vector<kp19pp::exception> seq;
    };
}

#endif // KP19PP_EXCEPTION_HPP_
