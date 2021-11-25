#include "mips.h"
#include "Inter.h"
#include "instructions.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
//#define now_func (func_stk.size() > 0 ?func_stk[func_stk.size()-1]:"global")
using namespace std;

extern ofstream mipsfile;
extern vector<Inter> interCode;
extern vector<string> str_const; //名字依次为str0,str1....
extern vector<RUN_TAB> running_symtable;
string now_func = "global";
int inter_index = 0;

vector<Inter> para_stk;
//      vector<string> func_stk = {"main"};

inline int get_func_mem(string name)
{
    auto &symtable = running_symtable[running_symtable.size() - 1].tab;
    Symble_item tem = Symble_item(name, VAR, INT);
    auto func_ = symtable.find(tem);
    if (func_ == symtable.end())
    {
        cout << "can't find "
             << "\'" << name << "\'"
             << "in symtable" << endl;
    }
    return func_->tot_memory;
}

Symble_item get_iden(string iden)
{
    for (int i = 0; i < running_symtable.size(); i++)
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

void load_from_mem(string iden, string reg)
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
    mipsfile << "enter: .asciiz \"\\n\"" << endl;
    int id = 0;
    for (auto &str : str_const)
    {
        mipsfile << "str" + to_string(id++) << ": .asciiz "
                 << "\""; //+ str + "\"" << endl;
        for (char a : str)
        {
            if (a == '\\')
                mipsfile << "\\";
            mipsfile << a;
        }
        mipsfile << "\"" << endl;
    }
    mipsfile << ".text" << endl;
    while (inter_index < interCode.size() and (interCode[inter_index].op == "=_const" or interCode[inter_index].op == "array_init"))
    {
        inter2mips(inter_index);
        inter_index++;
    }
    saveFp();
    int jkl = get_func_mem("main");
    changeSp(jkl + 4);
    jal("main");
    syscall(10);
}

inline void debug_in_symTab_interCode()
{
    ofstream interfile("intercode.txt");
    for (int i = 0; i < (int)interCode.size(); i++)
        interCode[i].out(interfile);
    interfile.close();
    ofstream symtablefile("sym_table.txt");
    for (RUN_TAB &tem : running_symtable)
    {
        symtablefile << "##:" << tem.name << endl;
        for (const Symble_item &jkl : tem.tab)
        {
            jkl.out(symtablefile);
        }
        symtablefile << endl;
    }
    symtablefile.close();
}

inline void opt_sll_sra()
{
    for (int i = 0; i < (int)interCode.size(); i++)
    {
        Inter& inter = interCode[i];
        if (inter.op == "*") {
            int id1 = -1, id2 = -1;
            int flag[2] = { 0,0 };
            if (check_str_is_int(inter.iden1))
            {
                id1 = stoi(inter.iden1);
                flag[0] = 1;
            }
            if (check_str_is_int(inter.iden2))
            {
                id2 = stoi(inter.iden2);
                flag[1] = 1;
            }
            if (flag[0] * flag[1] == 1)
            {
                interCode[i].op = "=_const";
                interCode[i].iden1 = to_string(id1 * id2);
                continue;
            }
            if (flag[1] == 1)
            {
                swap(interCode[i].iden1, interCode[i].iden2);
                swap(flag[0], flag[1]);
                swap(id1, id2);
            }
            if (flag[0] == 1)
            {
                int two = 0;
                int temid = id1;
                int tem_cnt = 0;
                while (temid > 0)
                {
                    if ((temid & 1) == 1)
                        tem_cnt++;
                    two++;
                    temid >>= 1;
                }
                if (id1 == 0)
                {
                    interCode[i].op = "=_const";
                    interCode[i].iden1 = to_string(0);
                }
                else if (id1 > 0 and tem_cnt == 1)
                {
                    two--;
                    interCode[i].op = "<<";
                    interCode[i].iden1 = interCode[i].iden2;
                    interCode[i].iden2 = to_string(two);
                }
            }
        }
        /*else if (inter.op == "/") {
            int id1 = -1, id2 = -1;
            int flag[2] = { 0,0 };
            if (check_str_is_int(inter.iden1))
            {
                id1 = stoi(inter.iden1);
                flag[0] = 1;
            }
            if (check_str_is_int(inter.iden2))
            {
                id2 = stoi(inter.iden2);
                flag[1] = 1;
            }
            if (flag[0] * flag[1] == 1)
            {
                interCode[i].op = "=_const";
                interCode[i].iden1 = to_string(id1 / id2);
                continue;
            }
            if (flag[1] == 1)
            {
                int two = 0;
                int temid = id2;
                int tem_cnt = 0;
                while (temid > 0)
                {
                    if ((temid & 1) == 1)
                        tem_cnt++;
                    two++;
                    temid >>= 1;
                }
                if (id2 > 0 and tem_cnt == 1)
                {
                    two--;
                    interCode[i].op = ">>";
                    //interCode[i].iden1 = interCode[i].iden2;
                    interCode[i].iden2 = to_string(two);
                }
            }
        }*/
    }
}

void start_inter2mips()
{
    ofstream innnn("intercode_ori.txt");
    for (int i = 0; i < (int)interCode.size(); i++)
        interCode[i].out(innnn);
    innnn.close();
    opt_sll_sra();
    debug_in_symTab_interCode();
    init();
    //syscall(10);
    //return;
    while (inter_index < interCode.size())
    {
        inter2mips(inter_index);
        inter_index++;
    }
}

bool check_str_is_int(string iden)
{
    if (iden == "")
        return false;
    int i = 0;
    if (iden[i] == '+' or iden[i] == '-')
        i++;
    for (; i < iden.size(); i++)
        if (!isdigit(iden[i]))
            return false;
    /*for (char a : iden)
        if (!isdigit(a))
            return false;*/
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
        now_func = now_code.iden1;
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
        la("$a0", "enter");
        syscall(4);
    }
    else if (now_code.op == "print_str_and_exp")
    {
        la("$a0", now_code.iden1);
        syscall(4);

        if (check_str_is_int(now_code.iden2))
        {
            li("$a0", stoi(now_code.iden2));
            if (now_code.tar == "char")
                syscall(11);
            else
                syscall(1);
        }
        else
        {
            Symble_item tem = get_iden(now_code.iden2);
            if (tem.name_space == "global") //全局变量
                lw("$a0", "global+" + to_string(tem.addr));
            else
                lw("$a0", to_string(-tem.addr), "$fp"); //注意这里是负数
            if (tem.data_type == INT and now_code.tar == "int")
                syscall(1);
            else
                syscall(11);
        }
        la("$a0", "enter");
        syscall(4);
    }
    else if (now_code.op == "print_int") //四种情况 //如果是常数，并且是字符，则其一定是标识符，所以如果是常数一定是int
    {
        if (check_str_is_int(now_code.iden1))
        {
            li("$a0", stoi(now_code.iden1));
            if (now_code.tar == "char")
                syscall(11);
            else
                syscall(1);
        }
        else
        {
            Symble_item tem = get_iden(now_code.iden1);
            if (tem.name_space == "global") //全局变量
                lw("$a0", "global+" + to_string(tem.addr));
            else
                lw("$a0", to_string(-tem.addr), "$fp"); //注意这里是负数
            if (now_code.tar == "char")
                syscall(11);
            else
                syscall(1);
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
    else if (now_code.op == "return")
    {
        // setSp();
        if (now_code.iden1 != "")
        {
            load_from_mem(now_code.iden1, "$v0");
        }
        jr("$ra");
    }
    else if (now_code.op == "array_init" || now_code.op == "save_arr_val") // 这个地方其实应该为了优化分开，但是不优化就合并到一起吧
    {
        load_from_mem(now_code.iden2, "$t0"); // offset
        load_from_mem(now_code.tar, "$t1");
        Symble_item tem = get_iden(now_code.iden1); //得到数组的sym
        if (tem.name_space == "global")             //全局数组
        {
            la("$t2", "global");
            li("$t3", tem.addr);
            add("$t2", "$t2", "$t3"); // 数组基地址
            add("$t2", "$t2", "$t0");
            sw("$t1", "0", "$t2");
            //lw(reg, "global+" + to_string(tem.addr));
        }
        else
        {
            // lw(reg, to_string(-tem.addr), "$fp");
            li("$t2", tem.addr);
            add("$t2", "$t2", "$t0");
            sub("$t2", "$fp", "$t2"); // t2是基地址
            sw("$t1", "0", "$t2");
        }
    }
    else if (now_code.op == "set_lab")
    {
        addLable(now_code.iden1);
    }
    else if (now_code.op == "get_arr_val")
    {
        load_from_mem(now_code.iden2, "$t0"); // offset
        //load_from_mem(now_code.tar, "$t1");
        Symble_item tem = get_iden(now_code.iden1); //得到数组的sym
        if (tem.name_space == "global")             //全局数组
        {
            la("$t2", "global");
            li("$t3", tem.addr);
            add("$t2", "$t2", "$t3"); // 数组基地址
            add("$t2", "$t2", "$t0");
            lw("$t1", "0", "$t2");
            store_to_mem(now_code.tar, "$t1");
        }
        else
        {
            li("$t2", tem.addr);
            add("$t2", "$t2", "$t0");
            sub("$t2", "$fp", "$t2"); // t2是基地址

            lw("$t1", "0", "$t2");
            store_to_mem(now_code.tar, "$t1");
        }
    }
    else if (now_code.op == "save_reg")
    {
        sw("$fp", "0", "$sp");
        changeSp(4);
        sw("$ra", "0", "$sp");
        changeSp(4);
        //saveFp();
    }
    else if (now_code.op == "load_reg")
    {
        setSp();
        changeSp(-4);
        lw("$ra", "0", "$sp");
        changeSp(-4);
        lw("$fp", "0", "$sp");
    }
    else if (now_code.op == "get_ret")
    {
        store_to_mem(now_code.tar, "$v0");
    }
    else if (now_code.op == "call") // 保证了call前面一定是对应的参数压栈的中间代码，再前面一定是保存寄存器的代码
    {
        int jkl = get_func_mem(now_code.iden1);
        //changeSp(jkl + 4);
        int offseet = 0;
        for (Inter &para : para_stk)
        {
            load_from_mem(para.iden1, "$t0");
            // 这个时候fp千万不能改，要靠这个东西找压入stack的变量

            sw("$t0", to_string(-offseet), "$sp");
            //此时对于要进入的函数，sp才是栈底
            offseet += 4;
        }
        saveFp();
        changeSp(jkl + 4);
        para_stk.clear();
        jal(now_code.iden1);
        //now_func = now_code.iden1;
    }
    else if (now_code.op == "push_para")
    {
        para_stk.push_back(now_code);
    }
    else if (now_code.op == "jump")
    {
        j(now_code.iden1);
    }
    // 这些全是和0比较
    else if (now_code.op == "GRE") // > bgtz
    {
        load_from_mem(now_code.iden1, "$t0");
        bgtz(now_code.tar, "$t0");
    }
    else if (now_code.op == "LSS") // < bltz
    {
        load_from_mem(now_code.iden1, "$t0");
        bltz(now_code.tar, "$t0");
    }
    else if (now_code.op == "GEQ") // >= bgez
    {
        load_from_mem(now_code.iden1, "$t0");
        bgez(now_code.tar, "$t0");
    }
    else if (now_code.op == "LEQ") // <= blez
    {
        load_from_mem(now_code.iden1, "$t0");
        blez(now_code.tar, "$t0");
    }
    else if (now_code.op == "NEQ") // !=
    {
        load_from_mem(now_code.iden1, "$t0");
        if (now_code.iden2 == "")
            bne(now_code.tar, "$t0", "$0");
        else
        {
            load_from_mem(now_code.iden2, "$t1");
            bne(now_code.tar, "$t0", "$t1");
        }
    }
    else if (now_code.op == "EQL") // ==
    {
        load_from_mem(now_code.iden1, "$t0");
        if (now_code.iden2 == "")
            beq(now_code.tar, "$t0", "$0");
        else
        {
            load_from_mem(now_code.iden2, "$t1");
            beq(now_code.tar, "$t0", "$t1");
        }
    }
    else if (now_code.op == "<<")
    {
        load_from_mem(now_code.iden1, "$t0");
        sll("$t1", "$t0", now_code.iden2);
        store_to_mem(now_code.tar, "$t1");
    }
    else if (now_code.op == ">>")
    {
        load_from_mem(now_code.iden1, "$t0");
        sra("$t1", "$t0", now_code.iden2);
        store_to_mem(now_code.tar, "$t1");
    }
    mipsfile << "#---------" << now_code.op << "," << now_code.iden1 << "," << now_code.iden2 << "," << now_code.tar << endl;
}