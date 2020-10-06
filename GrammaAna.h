#include "LexAla.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
using namespace std;
class GrammaAna
{
public:
    void startGramAna();
    
    ifstream &file_in;
    ofstream &file_out;
    LexAnalyzer &lex_ana;
    GrammaAna(ifstream &ff_in, ofstream &ff_out, LexAnalyzer &lex) : file_in(ff_in), file_out(ff_out), lex_ana(lex) {}
private:
    bool top_programe();    //顶层程序
    bool const_define();    //常量定义
    int const_check();//对应语法中的<常量> ::= <整数>|<字符>
    bool var_define();  //变量定义
    bool var_init_def(int firType);
    bool var_noInit_def(int firType);
    bool var_statment();    //变量说明
    bool charOrInt();
    int array_check();  //检测是变量还是一维数组二维数组
    bool const_stat();  //常量说明
    bool const_def();   //常量定义
    bool integer_check(); // 对应整数
    bool func_define();     //有返回值的函数
    bool func_noRet_define();   //无返回值的函数
    bool main_define();    //main函数
    bool para_list();   //参数表
    bool mult_sentence(); // 复合语句
    bool sentence_list(); //语句列
    bool sentence_check(); // 语句
    bool whileFor_check();
    bool if_check();
    bool switch_check();
    bool callFuncRet_check();
    bool callFuncNoRet_check();
    bool assign_check();
    bool read_check();
    bool printf_check();
    bool scanf_check();
    bool ret_check();
    bool condition_check();
    bool expre_check();
    bool step_check();
    bool item_check();
    bool factor_check();
    bool condition_table(); // <情况表>
    bool default_check();
    bool sub_condition();
    bool value_para_list(); // <值参数表>
    bool statment_head();// <声明头部>
};