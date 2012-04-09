#include <iostream>
#include <cstdlib>
#include "exception.hpp"
#include "commandline_options.hpp"
#include "scanner.hpp"
#include "target.hpp"

int main(){
    int argc = 5;
    const char *argv[] = {
        "dummy.exe",
        "-c++",
        "-indent=space",
        "ifile.txt",
        "ofile.hpp"
    };

    try{
        kp19pp::commandline_options_type commandline_options;
        if(!commandline_options.get(argc, argv)){
            return -1;
        }
        std::ifstream ifile(commandline_options.ifile_path());
        if(!ifile){
            std::cerr << "ファイルの読み込みに失敗しました.\n";
            return -1;
        }
        kp19pp::scanner::scanner_type scanner;
        scanner.scan(ifile);
        kp19pp::target::target_type target;
        bool result_make_parsing_table = target.make_parsing_table(scanner, commandline_options);
        if(!result_make_parsing_table){
            return -1;
        }
    }catch(kp19pp::exception e){
        std::cerr << "error " << (e.line_num + 1) << ":" << (e.char_num + 1) << ":" << e.what() << "\n";
        return -1;
    }catch(kp19pp::exception_seq e){
        std::cerr << "error " << e.what() << "\n";
        for(auto iter = e.seq.begin(), end = e.seq.end(); iter != end; ++iter){
            std::cerr << (iter->line_num + 1) << ":" << (iter->char_num + 1) << ":" << iter->what() << "\n";
        }
        return -1;
    }catch(...){
        std::cerr << "未知のエラーが発生しました.\n";
        return -1;
    }

    return 0;
}
