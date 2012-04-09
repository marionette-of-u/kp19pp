#include <iostream>
#include <cstdlib>
#include <locale>
#include "commandline_options.hpp"

namespace kp19pp{
    commandline_options_type::commandline_options_type() :
        ifile_path_(), ofile_path_(), language_(language_cpp), indent_(indent_space4)
    {}

    namespace{
        std::string str_to_upper(const char *str){
            std::locale l;
            std::string ret;
            for(std::size_t i = 0; str[i]; ++i){
                ret += std::toupper(str[i], l);
            }
            return ret;
        }
    }

    bool commandline_options_type::get(int argc, const char **argv){
        int state = 0;
        for(int index = 1; index < argc; ++index){
            if(argv[index][0] == '-'){
                std::string str = str_to_upper(argv[index]);
                if(
                    str == "-C++" ||
                    str == "-CPP"
                ){
                    language_ = language_cpp;
                    continue;
                }
                if(
                    str == "-C#" ||
                    str == "-CS" ||
                    str == "-CSHARP"
                ){
                    language_ = language_csharp;
                    continue;
                }
                if(str == "-D"){
                    language_ = language_d;
                    continue;
                }
                if(str == "-JAVA"){
                    language_ = language_java;
                }
                if(
                    str == "-JS" ||
                    str == "-JAVASCRIPT"
                ){
                    language_ = language_javascript;
                    continue;
                }
                if(str == "-INDENT=SPACE"){
                    indent_ = indent_space;
                    continue;
                }
                if(str == "-INDENT=SPACE4"){
                    indent_ = indent_space4;
                    continue;
                }
                if(str == "-INDENT=SPACE8"){
                    indent_ = indent_space8;
                    continue;
                }
                if(str == "-INDENT=TAB"){
                    indent_ = indent_tab;
                    continue;
                }
                std::cerr << "unknown options" << argv[index] << "\n";
                return false;
            }
            switch(state){
            case 0: ifile_path_ = argv[index]; ++state; break;
            case 1: ofile_path_ = argv[index]; ++state; break;
            default:
                std::cerr << "too many arguments\n";
                return false;
            }
        }
        if(state < 2){
            std::cout << "kp19pp usage: kp19pp [ -c++ | -cs | -d | -java | -javascript | -indent=space | -indent=tab ] ifile_name ofile_name\n";
            return false;
        }
        return true;
    }

    const std::string &commandline_options_type::ifile_path() const{
        return ifile_path_;
    }

    std::string commandline_options_type::ifile_name() const{
        std::string ret;
        for(std::size_t i = 0, length = ifile_path_.length(); i < length; ++i){
            char c = ifile_path_[length - i - 1];
            if(c == '/' || c == '\\'){ break; }
            ret += c;
        }
        return ret;
    }

    const std::string &commandline_options_type::ofile_path() const{
        return ofile_path_;
    }

    commandline_options_type::language_enum commandline_options_type::language() const{
        return language_;
    }

    commandline_options_type::indent_enum commandline_options_type::indent() const{
        return indent_;
    }
}
