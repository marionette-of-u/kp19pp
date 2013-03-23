#include <iostream>
#include <algorithm>
#include "common.hpp"
#include "commandline_options.hpp"

namespace kp19pp{
    commandline_options_type::commandline_options_type() :
        ifile_path_(), ofile_path_(), language_(language_cpp), indent_(indent_space4), time_(false), alltime_(false), log_(false)
    {}

    bool commandline_options_type::get(int argc, char *argv[]){
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
                if(
                    str == "-VIM" ||
                    str == "-VIMSCRIPT"
                ){
                    language_ = language_vimscript;
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
                if(str == "-TIME"){ 
                    time_ = true; 
                    continue; 
                } 
                if(str == "-ALLTIME"){ 
                    alltime_ = true; 
                    continue; 
                } 
                if(str == "-LOG"){ 
                    log_ = true; 
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
            std::cout << "kp19pp usage: kp19pp [ -c++ | -cs | -d | -java | -javascript | -vimscript | -indent=space | -indent=tab ] ifile_name ofile_name\n";
            return false;
        }
        return true;
    }

    const std::string &commandline_options_type::ifile_path() const{
        return ifile_path_;
    }

    std::string commandline_options_type::ifile_name() const{
        return file_name(ifile_path_);
    }

    const std::string &commandline_options_type::ofile_path() const{
        return ofile_path_;
    }

    std::string commandline_options_type::ofile_name() const{
        return file_name(ofile_path_);
    }

    std::string commandline_options_type::file_name(const std::string &str) const{
        std::string ret;
        for(std::size_t i = 0, length = str.size(); i < length; ++i){
            char c = str[length - i - 1];
            if(c == '/' || c == '\\'){ break; }
            ret += c;
        }
        std::reverse(ret.begin(), ret.end());
        return ret;
    }

    commandline_options_type::language_enum commandline_options_type::language() const{
        return language_;
    }

    commandline_options_type::indent_enum commandline_options_type::indent() const{
        return indent_;
    }

    bool commandline_options_type::time() const{
        return time_;
    }

    bool commandline_options_type::alltime() const{
        return alltime_;
    }

    bool commandline_options_type::log() const{
        return log_;
    }
}
