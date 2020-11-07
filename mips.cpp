#include "mips.h"
#include "Inter.h"
#include "instructions.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

extern ofstream mipsfile;
extern vector<Inter> interCode;
extern vector<string> str_const; //名字依次为str0,str1....
extern vector<RUN_TAB> running_symtable;
string now_func = "main";
int inter_index = 0;

inline int get_func_mem(string name)
{
    auto &symtable = running_symtable[running_symtable.size() - 1].tab;
    Symble_item tem = Symble_item(name, VAR, INT);
    auto func_ = symtable.find(tem);
    return func_->tot_memory;
}

Symble_item get_iden(string iden)
{
    for (int i = 1; i < running_symtable.size(); i++)
    {
        if (running_symtable[i].name == now_func)
        {
            auto &temtable = running_symtable[i].tab;
            auto tem_iter = temtable.find(Symble_item(iden, VAR, INT));
            if (tem_iter != temtable.end())
            {
                Symble_item tem = *tem_iter;
                tem.name_space = now_func;
                return tem;
            }
        }
    }
    auto &temtable = running_symtable[running_symtable.size() - 1].tab;
    auto tem_iter = temtable.find(Symble_item(iden, VAR, INT));
    if (tem_iter != temtable.end())
    {
        Symble_item tem = *tem_iter;
        tem.name_space = "global";
        return tem;
    }
    cout << "error in get iden" << endl;  
}

void store_to_mem(string iden, string reg)
{
    Symble_item tem = get_iden(iden);
    if (tem.name_space == "global") //全局变量
    {
        sw(reg, "global+" + to_string(tem.addr));
    }
    else
    {
        sw(reg, to_string(-tem.addr), "$fp");
    }
}

void load_from_mem(string iden,string reg)
{
    if (check_str_is_int(iden))
    {
        li(reg, stoi(iden));
        return;
    }
    Symble_item tem = get_iden(iden);
    if (tem.name_space == "global") //全局变量
    {
        lw(reg, "global+" + to_string(tem.addr));
    }
    else
    {
        lw(reg, to_string(-tem.addr), "$fp");
    }
}

inline void init()
{
    mipsfile << ".data" << endl;
    mipsfile << "global: .space 10000" << endl;
    int id = 0;
    for (auto &str : str_const)
    {
        mipsfile << "str" + to_string(id++) << ": .asciiz "
                 << "\"" + str + "\\n" + "\"" << endl;
    }
    mipsfile << ".text" << endl;
    while (interCode[inter_index].op != "func" and inter_index < interCode.size())
    {
        inter2mips(inter_index);
        inter_index++;
    }
    saveFp();
    changeSp(get_func_mem("main") + 4);
    jal("main");
    syscall(10);
}

void start_inter2mips()
{
    init();
    while (inter_index < interCode.size())
    {
        inter2mips(inter_index);
        inter_index++;
    }
}

bool check_str_is_int(string iden)
{
    for (char a : iden)
        if (!isdigit(a))
            return false;
    return true;
}

void inter2mips(int index)
{
    Inter &now_code = interCode[index];
    if (now_code.op == "=_const")
    {
        li("$t0", stoi(now_code.iden1));
        store_to_mem(now_code.tar, "$t0");
    }
    else if (now_code.op == "func")
    {
        addLable(now_code.iden1);
    }
    else if (now_code.op == "scanf")
    {
        Symble_item tem = get_iden(now_code.tar);
        if (tem.data_type == CHAR)
            syscall(12);
        else
            syscall(5);
        if (tem.name_space == "global") //全局变量
            sw("$v0", "global+" + to_string(tem.addr));
        else
            sw("$v0", to_string(-tem.addr), "$fp"); //注意这里是负数
    }
    else if (now_code.op == "print_str")
    {
        la("$a0", now_code.iden1);
        syscall(4);
    }
    else if (now_code.op == "print_int") //四种情况 //如果是常数，并且是字符，则其一定是标识符，所以如果是常数一定是int
    {
        if (check_str_is_int(now_code.iden1))
        {
            li("$a0", stoi(now_code.iden1));
            syscall(1);
        }
        else
        {
            Symble_item tem = get_iden(now_code.iden1);
            if (tem.name_space == "global") //全局变量
                lw("$a0", "global+" + to_string(tem.addr));
            else
                lw("$a0", to_string(-tem.addr), "$fp"); //注意这里是负数
            if (tem.data_type == INT)
                syscall(1);
            else
                syscall(11);
        }
        li("$a0", 10);
        syscall(11);
    }
    else if (now_code.op == "+")
    {
        load_from_mem(now_code.iden1, "$t0");
        load_from_mem(now_code.iden2, "$t1");
        add("$t2", "$t0", "$t1");
        store_to_mem(now_code.tar, "$t2");
    }
    else if (now_code.op == "-")
    {
        load_from_mem(now_code.iden1, "$t0");
        load_from_mem(now_code.iden2, "$t1");
        sub("$t2", "$t0", "$t1");
        store_to_mem(now_code.tar, "$t2");
    }
    else if (now_code.op == "*")
    {
        load_from_mem(now_code.iden1, "$t0");
        load_from_mem(now_code.iden2, "$t1");
        mult("$t2", "$t0", "$t1");
        store_to_mem(now_code.tar, "$t2");
    }
    else if (now_code.op == "/")
    {
        load_from_mem(now_code.iden1, "$t0");
        load_from_mem(now_code.iden2, "$t1");
        div("$t2", "$t0", "$t1");
        store_to_mem(now_code.tar, "$t2");
    }
    else if (now_code.op == "=")
    {
        load_from_mem(now_code.iden1, "$t0");
        store_to_mem(now_code.tar, "$t0");
    }
}
