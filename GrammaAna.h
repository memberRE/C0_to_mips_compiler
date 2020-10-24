#include "LexAla.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include "SymTable.h"
#include <string>
#include <vector>
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
    void add_error(int line_num,ERROR_TYPE__ e);
    bool top_programe();    //顶层程序
    bool const_define();    //常量定义
    int const_check();//对应语法中的<常量> ::= <整数>|<字符>
    bool var_define();  //变量定义
    // succ
    bool var_init_def(int firType,int dataType);
    bool var_noInit_def(int firType,int dataType);
    bool var_statment();    //变量说明
    //succ
    bool charOrInt();
    int array_check(int type);  //检测是变量还是一维数组二维数组
    //succ
    bool const_stat();  //常量说明
    //succ
    bool const_def();   //常量定义
    //succ
    int integer_check(); // 对应整数
    bool func_define();     //有返回值的函数
    bool func_noRet_define();   //无返回值的函数
    //succ
    bool main_define();    //main函数
    bool para_list(Symble_item & item);   //参数表
    int mult_sentence(int flag); // 复合语句
    // flag = 0:不需要返回值,flag = 1返回值为INT,flag = 2:返回值为CHAR
    int sentence_list(int flag); //语句列
    //同上
    int sentence_check(int flag); // 语句
    //同上
    bool whileFor_check();
    //succ
    bool if_check();
    bool switch_check();
    int callFuncRet_check();
    //succ
    bool callFuncNoRet_check();
    //succ
    bool assign_check();
    bool read_check();
    bool printf_check();
    bool scanf_check();
    bool ret_check(int flag);
    //succ
    bool condition_check();
    //succ
    int expre_check();
    //succ
    bool step_check();
    int item_check();
    int factor_check();
    bool condition_table(int type); // <情况表>
    bool default_check();
    bool sub_condition(int type);
    bool value_para_list(vector<TYPE_NAME> & real_para); // <值参数表>
    //succ
    bool statment_head();// <声明头部>
};