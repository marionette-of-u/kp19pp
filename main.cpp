#include <iostream>
#include "exception.hpp"
#include "scanner.hpp"
#include "target.hpp"

int main(){
    try{
        std::ifstream ifile("ifile.txt");
        if(!ifile){
            std::cerr << "ファイルの読み込みに失敗しました.\n";
            return -1;
        }
        kp19pp::scanner::scanner_type scanner;
        scanner.scan(ifile);
        kp19pp::target::target_type target;
        target.make_parsing_table(scanner);
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
