#include <iostream>
#include "exception.hpp"
#include "scanner.hpp"

void example();

int main(){
    //example();

    std::ifstream ifile("ifile.txt");
    if(!ifile){
        std::cerr << "ファイルの読み込みに失敗しました.\n";
        return -1;
    }
    try{
        kp19pp::scanner::scanner.scan(ifile);
    }catch(kp19pp::exception e){
        std::cerr << e.what() << "\n" << "line num : " << (e.line_num + 1) << ", char num : " << (e.char_num + 1) << "\n";
        return -1;
    }catch(kp19pp::exception_seq e){
        std::cerr << e.what() << "\n";
        for(auto iter = e.seq.begin(), end = e.seq.end(); iter != end; ++iter){
            std::cerr << iter->what() << "\n" << "line num : " << (iter->line_num + 1) << ", char num" << (iter->char_num + 1) << "\n";
        }
        return -1;
    }catch(...){
        std::cerr << "未知のエラーが発生しました.\n";
        return -1;
    }

    std::cout << "正常終了\n";

    return 0;
}
