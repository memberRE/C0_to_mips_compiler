#include "GrammaAna.h"
#include <map>
#include <set>
#include <vector>
#include <wchar.h>
#define ERROR_HANDLAR
using namespace std;
extern symType now_type;
extern string str_read;
extern string lower_str_read;
extern int line_num; // line number
extern map<string, symType> str2type;
extern map<symType, string> type2str;
extern vector<SYM_INFO> lexRes;
//extern vector<pair<symType, string>> lexRes;

int arr_dime[2];     //储存读到的数组的每一位的size，用来判断是否初始化出错
int ret_in_func = 0; // 在函数定义的里面是否有返回语句
int need_ret_type = 0;
vector<symType> sym_stk;
vector<set<Symble_item>> sym_table_stk; // 符号表
set<string> noRetFunc_symSet;
int sym_top = 0, sym_index = 0; //sym_index指向第一个未处理的sym
int out_index = 0;              //out_index指向下一个需要输出的sym

#define NOWSYM (sym_index < (int)lexRes.size() ? lexRes[sym_index].first : EOFTK)
#define NOWLINE (lexRes[sym_index].line_num)
#define PEEKSYM(x) (sym_index + x < (int)lexRes.size() ? lexRes[sym_index + x].first : EOFTK)
#define LAS_LINENUM ((sym_index - 1 < 0) ? 1 : lexRes[sym_index - 1].line_num)
#define NOWSTR lexRes[sym_index].second
#define POP_SYMSTK                     \
    {                                  \
        if (sym_table_stk.size() != 0) \
            sym_table_stk.pop_back();  \
    }
#define NEW_SYMSTK                          \
    {                                       \
        set<Symble_item> sub_table;         \
        sym_table_stk.push_back(sub_table); \
    }
#define NOW_SYMTAB (sym_table_stk[sym_table_stk.size() - 1])

#define NEXTSYM                               \
    {                                         \
        if (sym_top < sym_stk.size())         \
            now_type = sym_stk[sym_top];      \
        else                                  \
            lex_ana.get_sym(), lex_ana.OUT(); \
    }

#define GRAOUT                        \
    {                                 \
        while (out_index < sym_index) \
        {                             \
            lex_ana.OUT(out_index);   \
            out_index++;              \
        }                             \
    }

#define IDENFR_EXIST_CHECK                                              \
    Symble_item tem_sym = Symble_item(NOWSTR, VAR, INT);                \
    int if_found = 1;                                                   \
    auto id_sym = NOW_SYMTAB.find(tem_sym);                             \
    if (id_sym == NOW_SYMTAB.end() and sym_table_stk.size() > 1)        \
    {                                                                   \
        id_sym = sym_table_stk[sym_table_stk.size() - 2].find(tem_sym); \
        if (id_sym == sym_table_stk[sym_table_stk.size() - 2].end())    \
            if_found = 0;                                               \
    }                                                                   \
    else if (id_sym == NOW_SYMTAB.end())                                \
        if_found = 0;                                                   \
    if (if_found == 0)                                                  \
        add_error(LAS_LINENUM, NAME_NODEF);

#define IDENFR_EXIST_CONST_CHECK                                        \
    Symble_item tem_sym = Symble_item(NOWSTR, VAR, INT);                \
    int if_found = 1;                                                   \
    auto id_sym = NOW_SYMTAB.find(tem_sym);                             \
    if (id_sym == NOW_SYMTAB.end() and sym_table_stk.size() > 1)        \
    {                                                                   \
        id_sym = sym_table_stk[sym_table_stk.size() - 2].find(tem_sym); \
        if (id_sym == sym_table_stk[sym_table_stk.size() - 2].end())    \
            if_found = 0;                                               \
    }                                                                   \
    else if (id_sym == NOW_SYMTAB.end())                                \
        if_found = 0;                                                   \
    if (if_found == 0)                                                  \
        add_error(LAS_LINENUM, NAME_NODEF);                             \
    else if (id_sym->iden_type == CONSTT)                               \
        add_error(LAS_LINENUM, CONST_WRI_ERR);

inline int toInt(string &str)
{
    int x = 0;
    if (str.size() == 1)
        return (int)str[0];
    else
        for (int i = 0; i < str.size(); i++)
        {
            x *= 10;
            x += str[i] - '0';
        }
    return x;
}

/*
    ＜整数＞        ::= ［＋｜－］＜无符号整数＞
*/
int GrammaAna::integer_check()
{
    int x = 1;
    if (NOWSYM == PLUS or NOWSYM == MINU)
    {
        if (NOWSYM == MINU)
            x = -1;
        sym_index++;
    }
    if (NOWSYM == INTCON)
    {
        sym_index++;
        GRAOUT;
        file_out << "<无符号整数>" << endl;
        cout << "<无符号整数>" << endl;
        x *= toInt(NOWSTR);
    }
    else
    {
        //Error
        cout << "Error in func integer_check" << endl;
    }
    GRAOUT;
#ifndef ERROR_HANDLAR
    file_out << "<整数>" << endl;
#endif
    cout << "<整数>" << endl;
    return x;
}

/*
＜常量定义＞::=int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}  | char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞} 
*/
bool GrammaAna::const_def() // finish
{
    int flag = NOWSYM == INTTK ? 1 : 2;
    if (NOWSYM != INTTK and NOWSYM != CHARTK)
    {
        //Error
        cout << "Error in func const_def" << endl;
        return false;
    }
    do
    {
        int tem_line_num = 0; // 出现重名的行数
        sym_index++;
        if (NOWSYM != IDENFR)
        {
            //Error
            cout << "Error in func const_def2" << endl;
            return false;
        }
        tem_line_num = NOWLINE;
        Symble_item tem_idenfr = Symble_item(NOWSTR, CONSTT, flag == 1 ? INT : CHAR);
        sym_index++;
        if (NOWSYM != ASSIGN)
        {
            //Error
            cout << "Error in func const_def3" << endl;
            return false;
        }
        sym_index++;
        if (flag == 1)
        {
            int const_val = integer_check();
            tem_idenfr.const_val = const_val;
        }
        else
        {
            if (NOWSYM != CHARCON)
            {
                //Error
                cout << "Error in func const_def6" << endl;
                return false;
            }
            tem_idenfr.const_val = toInt(NOWSTR);
            sym_index++;
        }
        //----ErrCheck
        if (NOW_SYMTAB.find(tem_idenfr) == NOW_SYMTAB.end())
        {
            //---succ
            NOW_SYMTAB.insert(tem_idenfr);
        }
        else
        {
            add_error(tem_line_num, NAME_REDEF);
        }
        //-----
    } while (NOWSYM == COMMA);
    GRAOUT;
#ifndef ERROR_HANDLAR
    file_out << "<常量定义>" << endl;
#endif
    cout << "<常量定义>" << endl;
    return true;
}
/*
    ＜常量说明＞ ::=  const＜常量定义＞;{ const＜常量定义＞;}
*/
bool GrammaAna::const_stat()
{
    do
    {
        if (NOWSYM != CONSTTK)
            break;
        sym_index++;
        const_def();
        if (NOWSYM != SEMICN)
        {
            //Error
            add_error(LAS_LINENUM, NO_SEM);
            cout << "Error in func const_stat" << endl;
        }
        else
            sym_index++;
    } while (1);
    GRAOUT;
    cout << "<常量说明>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<常量说明>" << endl;
#endif
    return true;
}

/*
＜常量＞   ::=  ＜整数＞|＜字符＞
返回值： 1字符；0整数
*/
int GrammaAna::const_check()
{
    if (NOWSYM == CHARCON)
    {
        sym_index++;
        GRAOUT;
        cout << "<常量>" << endl;
#ifndef ERROR_HANDLAR
        file_out << "<常量>" << endl;
#endif
        return 1;
    }
    else
    {
        integer_check();
        GRAOUT;
        cout << "<常量>" << endl;
#ifndef ERROR_HANDLAR
        file_out << "<常量>" << endl;
#endif
        return 0;
    }
}

/*
    ＜变量定义及初始化＞  ::= ＜类型标识符＞＜标识符＞=＜常量＞ |
    ＜类型标识符＞＜标识符＞'['＜无符号整数＞']'='{'＜常量＞{,＜常量＞}'}' |
    ＜类型标识符＞＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']'='{''{'＜常量＞{,＜常量＞}'}'{, '{'＜常量＞{,＜常量＞}'}'}'}'
*/
bool GrammaAna::var_init_def(int firType, int dataType) // 0 int ,1 char
{
    if (firType == 1) //变量
    {
        int get_type = const_check();
        if (get_type != dataType)
        {
            add_error(LAS_LINENUM, TYPE_NOT_MATCH);
        }
    }
    else if (firType == 2) //一维数组
    {
        if (NOWSYM != LBRACE) //{
        {
            cout << "Error in func var_init_def" << endl;
            return 0;
        }
        int arr_1d_num = 0;
        do
        {
            sym_index++;
            int get_type = const_check();
            arr_1d_num++;
            if (get_type != dataType)
            {
                add_error(LAS_LINENUM, TYPE_NOT_MATCH);
            }
        } while (NOWSYM == COMMA);
        if (NOWSYM != RBRACE) //{
        {
            cout << "Error in func var_init_def" << endl;
            return 0;
        }
        if (arr_1d_num != arr_dime[0])
        {
            add_error(LAS_LINENUM, ARRAY_INIT_ERR);
        }
        sym_index++;
    }
    else if (firType == 3)
    {
        int arr_1d_num = 0;
        if (NOWSYM != LBRACE) //{
        {
            cout << "Error in func var_init_def" << endl;
            return 0;
        }
        do
        {
            arr_1d_num++;
            sym_index++;
            if (NOWSYM != LBRACE) //{
            {
                cout << "Error in func var_init_def" << endl;
                return 0;
            }
            int arr_2d_num = 0;
            do
            {
                sym_index++;
                arr_2d_num++;
                int get_type = const_check();
                if (get_type != dataType)
                {
                    add_error(LAS_LINENUM, TYPE_NOT_MATCH);
                }
            } while (NOWSYM == COMMA);
            if (NOWSYM != RBRACE) //}
            {
                cout << "Error in func var_init_def" << endl;
                return 0;
            }
            if (arr_2d_num != arr_dime[1])
            {
                add_error(LAS_LINENUM, ARRAY_INIT_ERR);
            }
            sym_index++;
        } while (NOWSYM == COMMA);
        if (NOWSYM != RBRACE) //{
        {
            cout << "Error in func var_init_def" << endl;
            return 0;
        }
        if (arr_1d_num != arr_dime[0])
        {
            add_error(LAS_LINENUM, ARRAY_INIT_ERR);
        }
        sym_index++;
    }
    else
    {
        cout << "........" << endl;
        return 0;
    }
    GRAOUT;
    cout << "<变量定义及初始化>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<变量定义及初始化>" << endl;
#endif
    return 1;
}

/*
    注意这个可以有多个用逗号隔开
    ＜变量定义无初始化＞  ::= ＜类型标识符＞
    (
        ＜标识符＞|
        ＜标识符＞'['＜无符号整数＞']'|
        ＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']'
        )
        {,(＜标识符＞|＜标识符＞'['＜无符号整数＞']'|＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']' )}
*/

bool GrammaAna::var_noInit_def(int firType, int type)
{
    while (NOWSYM == COMMA)
    {
        sym_index++;
        if (!array_check(type))
            return 0;
    }
    GRAOUT;
    cout << "<变量定义无初始化>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<变量定义无初始化>" << endl;
#endif
    return 1;
}

// succ
int GrammaAna::array_check(int type) //这个地方出现的错误可能是：重复定义，缺少']'
{
    Symble_item tem_symble = Symble_item(NOWSTR, VAR, (TYPE_NAME)type);
    int name_line = NOWLINE;
    //这里在后面加了peek3主要是因为错误处理有可能数组会少右括号
    if (NOWSYM == IDENFR and PEEKSYM(1) == LBRACK and (PEEKSYM(4) == LBRACK or PEEKSYM(3) == LBRACK))
    {
        // 二维数组 a[x][y]
        tem_symble.iden_type = ARRAY;
        tem_symble.dimen = 2;
        sym_index += 2;
        if (NOWSYM != INTCON)
        {
            add_error(LAS_LINENUM, INDEX_ERR);
        }
        else
        {
            tem_symble.dimen_size[0] = toInt(NOWSTR);
        }
        sym_index++;
        if (NOWSYM != RBRACK)
        {
            add_error(LAS_LINENUM, NO_RBRACK);
            sym_index--; // 为了不用改下面的sym_index+2;
        }
        GRAOUT;
        cout << "<无符号整数>" << endl;
#ifndef ERROR_HANDLAR
        file_out << "<无符号整数>" << endl;
#endif
        sym_index += 2;
        if (NOWSYM != INTCON)
        {
            add_error(LAS_LINENUM, INDEX_ERR);
        }
        else
        {
            tem_symble.dimen_size[1] = toInt(NOWSTR);
        }
        sym_index++;
        GRAOUT;
        cout << "<无符号整数>" << endl;
#ifndef ERROR_HANDLAR
        file_out << "<无符号整数>" << endl;
#endif
        if (NOWSYM != RBRACK) // no ]
        {
            add_error(LAS_LINENUM, NO_RBRACK);
        }
        else
            sym_index++;
        if (NOW_SYMTAB.find(tem_symble) == NOW_SYMTAB.end())
            NOW_SYMTAB.insert(tem_symble);
        else
        {
            add_error(name_line, NAME_REDEF);
        }
        arr_dime[0] = tem_symble.dimen_size[0];
        arr_dime[1] = tem_symble.dimen_size[1];
        return 3;
    }
    else if (NOWSYM == IDENFR and PEEKSYM(1) == LBRACK) //一维数组
    {
        tem_symble.iden_type = ARRAY;
        tem_symble.dimen = 1;
        sym_index += 2;
        if (NOWSYM != INTCON)
        {
            add_error(LAS_LINENUM, INDEX_ERR);
        }
        else
        {
            tem_symble.dimen_size[0] = toInt(NOWSTR);
        }
        sym_index++;
        GRAOUT;
        cout << "<无符号整数>" << endl;
#ifndef ERROR_HANDLAR
        file_out << "<无符号整数>" << endl;
#endif
        if (NOWSYM != RBRACK)
        {
            add_error(LAS_LINENUM, NO_RBRACK);
        }
        else
            sym_index++;
        if (NOW_SYMTAB.find(tem_symble) == NOW_SYMTAB.end())
            NOW_SYMTAB.insert(tem_symble);
        else
        {
            add_error(name_line, NAME_REDEF);
        }
        arr_dime[0] = tem_symble.dimen_size[0];
        return 2;
    }
    else if (NOWSYM == IDENFR) // 变量
    {
        sym_index++;
        if (NOW_SYMTAB.find(tem_symble) == NOW_SYMTAB.end())
            NOW_SYMTAB.insert(tem_symble);
        else
        {
            add_error(name_line, NAME_REDEF);
        }
        return 1;
    }
}
/*
    ＜变量定义＞ ::= ＜变量定义无初始化＞|＜变量定义及初始化＞
*/
bool GrammaAna::var_define() // succ
{
    int datatype = 0;
    if (NOWSYM == INTTK)
        datatype = 0;
    else if (NOWSYM == CHARTK)
        datatype = 1;
    /*
    if (NOWSYM != INTTK and NOWSYM != CHARTK)
    {
        cout << "Error in func var_define" << endl;
        return false;
    }
    */
    sym_index++; //现在指向标识符
    int type = array_check(datatype);
    if (type == 0)
        return false;
    if (NOWSYM == ASSIGN)
    {
        sym_index++;
        if (!var_init_def(type, datatype))
        {
            cout << "Error in func var_define" << endl;
            return 0;
        }
    }
    else if (!var_noInit_def(type, datatype))
    {
        cout << "Error in func var_define" << endl;
        return 0;
    }
    GRAOUT;
    cout << "<变量定义>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<变量定义>" << endl;
#endif
    return true;
}
/*
＜变量说明＞  ::= ＜变量定义＞;{＜变量定义＞;}
*/
bool GrammaAna::var_statment() // succ
{
    do
    {
        if (!var_define())
        {
            cout << "Error in func var_statment" << endl;
            return false;
        }
        if (NOWSYM != SEMICN)
        {
            //Error
            add_error(LAS_LINENUM, NO_SEM);
            cout << "Error in func var_statment" << endl;
        }
        else
            sym_index++;
    } while (lexRes[sym_index + 2].first != LPARENT and (NOWSYM == INTTK or NOWSYM == CHARTK) and PEEKSYM(1) == IDENFR);
    GRAOUT;
    cout << "<变量说明>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<变量说明>" << endl;
#endif
    return true;
}
/*
    ＜参数表＞    ::=  ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}| ＜空＞
*/
bool GrammaAna::para_list(Symble_item &item)
{
    if (NOWSYM == RPARENT or NOWSYM == LBRACE) //因为有可能缺少右小括号
    {
        GRAOUT;
        cout << "<参数表>" << endl;
#ifndef ERROR_HANDLAR
        file_out << "<参数表>" << endl;
#endif
        return true;
    }
    sym_index--; // 这里主要为了和下面的dowhile配合才--
    do
    {
        sym_index++;
        if (NOWSYM != INTTK and NOWSYM != CHARTK)
        {
            cout << "Error in func para_list" << endl;
            return 0;
        }
        TYPE_NAME type_name = NOWSYM == INTTK ? INT : CHAR;
        item.para_list.push_back(type_name);
        sym_index++;
        if (NOWSYM != IDENFR)
        {
            cout << "Error in func para_list" << endl;
            return 0;
        }
        Symble_item tem_sym = Symble_item(NOWSTR, VAR, type_name);
        //---add to symTable
        if (NOW_SYMTAB.find(tem_sym) != NOW_SYMTAB.end())
        {
            add_error(NOWLINE, NAME_REDEF);
        }
        else
            NOW_SYMTAB.insert(tem_sym);
        sym_index++;
    } while (NOWSYM == COMMA);
    GRAOUT;
    cout << "<参数表>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<参数表>" << endl;
#endif
    return true;
}

/*
＜步长＞::= ＜无符号整数＞  
*/
bool GrammaAna::step_check()
{
    if (NOWSYM != INTCON)
    {
        cout << "Error in step_check" << endl;
        return 0;
    }
    sym_index++;
    GRAOUT;
    cout << "<无符号整数>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<无符号整数>" << endl;
#endif
    GRAOUT;
    cout << "<步长>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<步长>" << endl;
#endif
    return 1;
}

/*
＜条件＞    ::=  ＜表达式＞＜关系运算符＞＜表达式＞
*/
// succ
bool GrammaAna::condition_check()
{
    int typee;
    typee = expre_check();
    if (typee == 2)
    {
        add_error(LAS_LINENUM, CONDITION_TYPE_ERR);
    }

    if (!(NOWSYM == LSS or NOWSYM == LEQ or NOWSYM == GRE or NOWSYM == GEQ or NOWSYM == EQL or NOWSYM == NEQ))
    {
        cout << "Error in func condition_check" << endl;
        return 0;
    }
    sym_index++;
    typee = expre_check();
    if (typee == 2)
    {
        add_error(LAS_LINENUM, CONDITION_TYPE_ERR);
    }
    GRAOUT;
    cout << "<条件>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<条件>" << endl;
#endif
    return 1;
}

/*
＜表达式＞::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}  
*/
int GrammaAna::expre_check()
{
    int typee = 0; // 1int,2char -1 void
    if (NOWSYM == PLUS || NOWSYM == MINU)
    {
        sym_index++;
        typee = 1;
    }
    int tem = item_check();
    typee = typee == 1 ? typee : tem;
    if (tem == -1)
        typee = -1;
    while (NOWSYM == PLUS || NOWSYM == MINU) //fuck！！！ 加法运算符居然包括减法
    {
        sym_index++;
        if (typee != -1)
            typee = 1;
        tem = item_check();
        if (tem == -1)
            typee = -1;
    }
    GRAOUT;
    cout << "<表达式>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<表达式>" << endl;
#endif
    return typee;
}

/*
＜项＞::= ＜因子＞{＜乘法运算符＞＜因子＞}
*/
int GrammaAna::item_check()
{
    int typee = 0;
    typee = factor_check();
    while (NOWSYM == MULT || NOWSYM == DIV) //只要参与运算就会被变成int
    {
        sym_index++;
        if (typee != -1)
            typee = 1;
        if (factor_check() == -1)
            typee = -1;
    }
    GRAOUT;
    cout << "<项>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<项>" << endl;
#endif
    return typee;
}

/*
＜因子＞    ::= 
＜标识符＞｜
＜标识符＞'['＜表达式＞']'|
＜标识符＞'['＜表达式＞']''['＜表达式＞']'|
'('＜表达式＞')'｜
＜整数＞|
＜字符＞｜
＜有返回值函数调用语句＞
*/
//succ
int GrammaAna::factor_check()
{
    int typee = 0;
    if (NOWSYM == LPARENT) // '('
    {
        sym_index++;
        typee = expre_check();
        if (NOWSYM != RPARENT) // )
        {
            add_error(LAS_LINENUM, NO_RPARENT);
        }
        else
            sym_index++;
    }
    else if (NOWSYM == CHARCON)
    {
        typee = 2;
        sym_index++;
    }
    else if (NOWSYM == IDENFR and PEEKSYM(1) != LPARENT) //说明不是函数调用
    {
        Symble_item tem_sym = Symble_item(NOWSTR, VAR, INT);
        sym_index++;
        int if_findd = 1;
        auto id_sym = NOW_SYMTAB.find(tem_sym);
        if (id_sym == NOW_SYMTAB.end())
        {
            if (sym_table_stk.size() > 1)
            {
                id_sym = sym_table_stk[sym_table_stk.size() - 2].find(tem_sym);
                if (id_sym == sym_table_stk[sym_table_stk.size() - 2].end())
                    if_findd = 0;
            }
            else
                if_findd = 0;
        }
        if (if_findd == 0)
            typee = -1;
        else
            typee = id_sym->data_type == INT ? 1 : 2;
        if (NOWSYM == LBRACK) // [
        {
            sym_index++;
            int tem_typee = expre_check();
            if (tem_typee == 2) //即不是未定义和int，是char
            {
                add_error(LAS_LINENUM, INDEX_ERR);
            }
            if (NOWSYM != RBRACK)
            {
                add_error(LAS_LINENUM, NO_RBRACK);
            }
            else
                sym_index++;
            if (NOWSYM == LBRACK)
            {
                sym_index++;
                tem_typee = expre_check();
                if (tem_typee == 2) //即不是未定义和int，是char
                {
                    add_error(LAS_LINENUM, INDEX_ERR);
                }
                if (NOWSYM != RBRACK)
                {
                    add_error(LAS_LINENUM, NO_RBRACK);
                }
                else
                    sym_index++;
            }
        }
    }
    else if (NOWSYM == IDENFR and PEEKSYM(1) == LPARENT)
        typee = callFuncRet_check();
    else
    {
        typee = 1;
        integer_check();
    }
    GRAOUT;
    cout << "<因子>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<因子>" << endl;
#endif
    return typee;
}

/*
＜循环语句＞   ::=  
while '('＜条件＞')'＜语句＞ | 
for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞     
*/

bool GrammaAna::whileFor_check()
{
#define ERROR_                                          \
    {                                                   \
        cout << "Error in func whileFor_check" << endl; \
        return 0;                                       \
    }
    if (NOWSYM == WHILETK)
    {
        sym_index++;
        if (NOWSYM != LPARENT)
            ERROR_
        sym_index++;
        condition_check();
        if (NOWSYM != RPARENT)
        {
            add_error(LAS_LINENUM, NO_RPARENT);
        }
        else
            sym_index++;
        sentence_check(0); // 这个地方flag参数实际上已经被弃用，没必要加了
    }
    else if (NOWSYM == FORTK)
    {
        sym_index++;
        if (NOWSYM != LPARENT)
            ERROR_
        sym_index++;
        if (NOWSYM != IDENFR)
            ERROR_

        Symble_item tem_sym = Symble_item(NOWSTR, VAR, INT);
        int if_found = 1;
        auto id_sym = NOW_SYMTAB.find(tem_sym);
        if (id_sym == NOW_SYMTAB.end() and sym_table_stk.size() > 1)
        {
            id_sym = sym_table_stk[sym_table_stk.size() - 2].find(tem_sym);
            if (id_sym == sym_table_stk[sym_table_stk.size() - 2].end())
                if_found = 0;
        }
        else if (id_sym == NOW_SYMTAB.end())
            if_found = 0;

        if (if_found == 0)
        {
            add_error(LAS_LINENUM, NAME_NODEF);
        }
        else if (id_sym->iden_type == CONSTT)
        {
            add_error(LAS_LINENUM, CONST_WRI_ERR);
        }

        sym_index++;
        if (NOWSYM != ASSIGN)
            ERROR_
        sym_index++;
        expre_check();
        if (NOWSYM != SEMICN)
        {
            add_error(LAS_LINENUM, NO_SEM);
        }
        else
            sym_index++;

        condition_check();

        if (NOWSYM != SEMICN)
        {
            add_error(LAS_LINENUM, NO_SEM);
        }
        else
            sym_index++;
        if (NOWSYM != IDENFR)
            ERROR_
        //------------------------------------------id_check
        tem_sym = Symble_item(NOWSTR, VAR, INT);
        if_found = 1;
        id_sym = NOW_SYMTAB.find(tem_sym);
        if (id_sym == NOW_SYMTAB.end() and sym_table_stk.size() > 1)
        {
            id_sym = sym_table_stk[sym_table_stk.size() - 2].find(tem_sym);
            if (id_sym == sym_table_stk[sym_table_stk.size() - 2].end())
                if_found = 0;
        }
        else if (id_sym == NOW_SYMTAB.end())
            if_found = 0;

        if (if_found == 0)
        {
            add_error(LAS_LINENUM, NAME_NODEF);
        }
        else if (id_sym->iden_type == CONSTT)
        {
            add_error(LAS_LINENUM, CONST_WRI_ERR);
        }
        //---------------------------------------------------
        sym_index++;
        if (NOWSYM != ASSIGN)
            ERROR_
        sym_index++;
        if (NOWSYM != IDENFR)
            ERROR_
        //------------------------------------------id_check
        tem_sym = Symble_item(NOWSTR, VAR, INT);
        if_found = 1;
        id_sym = NOW_SYMTAB.find(tem_sym);
        if (id_sym == NOW_SYMTAB.end() and sym_table_stk.size() > 1)
        {
            id_sym = sym_table_stk[sym_table_stk.size() - 2].find(tem_sym);
            if (id_sym == sym_table_stk[sym_table_stk.size() - 2].end())
                if_found = 0;
        }
        else if (id_sym == NOW_SYMTAB.end())
            if_found = 0;

        if (if_found == 0)
        {
            add_error(LAS_LINENUM, NAME_NODEF);
        }
        //---------------------------------------------------
        sym_index++;
        if (NOWSYM != PLUS and NOWSYM != MINU)
            ERROR_
        sym_index++;

        step_check();

        if (NOWSYM != RPARENT)
        {
            add_error(LAS_LINENUM, NO_RPARENT);
        }
        else
            sym_index++;

        sentence_check(0);
    }
    else
        ERROR_
    GRAOUT;
    cout << "<循环语句>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<循环语句>" << endl;
#endif
#undef ERROR_
    return 1;
}

/*
＜条件语句＞  ::= if '('＜条件＞')'＜语句＞［else＜语句＞］
*/
bool GrammaAna::if_check()
{
    if (NOWSYM != IFTK)
    {
        cout << "Error in ifcheck" << endl;
        return 0;
    }
    sym_index++;
    if (NOWSYM != LPARENT)
    {
        cout << "Error in ifcheck" << endl;
        return 0;
    }
    sym_index++;

    condition_check();

    if (NOWSYM != RPARENT)
    {
        add_error(LAS_LINENUM, NO_RPARENT);
    }
    else
        sym_index++;

    sentence_check(0);

    if (NOWSYM == ELSETK)
    {
        sym_index++;
        sentence_check(0);
    }

    GRAOUT
    cout << "<条件语句>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<条件语句>" << endl;
#endif
    return 1;
}

/*
＜读语句＞    ::=  scanf '('＜标识符＞')' 
succ
*/
bool GrammaAna::scanf_check()
{
    if (NOWSYM != SCANFTK)
    {
        cout << "Error in scanf" << endl;
        return 0;
    }
    sym_index++;
    if (NOWSYM != LPARENT)
    {
        cout << "Error in scanf" << endl;
        return 0;
    }
    sym_index++;
    if (NOWSYM != IDENFR)
    {
        cout << "Error in scanf" << endl;
        return 0;
    }

    IDENFR_EXIST_CONST_CHECK

    sym_index++;
    if (NOWSYM != RPARENT)
    {
        add_error(LAS_LINENUM, NO_RPARENT);
    }
    else
        sym_index++;

    GRAOUT;
    cout << "<读语句>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<读语句>" << endl;
#endif
    return 1;
}

/*
＜写语句＞    ::= 
printf '(' ＜字符串＞,＜表达式＞ ')'|
printf '('＜字符串＞ ')'|
printf '('＜表达式＞')' 
succ
*/
bool GrammaAna::printf_check()
{
    if (NOWSYM != PRINTFTK)
    {
        cout << "Error in printf" << endl;
        return 0;
    }
    sym_index++;
    if (NOWSYM != LPARENT)
    {
        cout << "Error in printf" << endl;
        return 0;
    }
    sym_index++;
    if (NOWSYM == STRCON)
    {
        sym_index++;
        GRAOUT;
        cout << "<字符串>" << endl;
#ifndef ERROR_HANDLAR
        file_out << "<字符串>" << endl;
#endif
        if (NOWSYM == COMMA)
        {
            sym_index++;
            expre_check();
        }
    }
    else
    {
        //sym_index++;
        expre_check();
    }
    if (NOWSYM != RPARENT)
    {
        add_error(LAS_LINENUM, NO_RPARENT);
    }
    else
        sym_index++;

    GRAOUT;
    cout << "<写语句>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<写语句>" << endl;
#endif
    return 1;
}

/*
＜返回语句＞   ::=  return['('＜表达式＞')']   
每次进行到这个函数，都会ret_in_func++
succ
*/
bool GrammaAna::ret_check(int flag)
{
    ret_in_func++;
    if (NOWSYM != RETURNTK)
    {
        cout << "Error in return" << endl;
        return 0;
    }
    sym_index++;
    if (NOWSYM == LPARENT)
    {
        sym_index++;
        if (NOWSYM == RPARENT) //确保表达式不为空
        {
            add_error(LAS_LINENUM, need_ret_type == 0 ? NORET_FUNC_ERR : RET_FUNC_ERR);
            sym_index++;
            return 1;
        }
        int temtype = expre_check();
        if (temtype != need_ret_type and temtype != -1)
        {
            add_error(LAS_LINENUM, need_ret_type == 0 ? NORET_FUNC_ERR : RET_FUNC_ERR);
        }
        if (NOWSYM != RPARENT)
        {
            add_error(LAS_LINENUM, NO_RPARENT);
        }
        else
            sym_index++;
    }
    else //return为空的时候
    {
        if (need_ret_type != 0)
            add_error(LAS_LINENUM, RET_FUNC_ERR);
    }
    GRAOUT;
    cout << "<返回语句>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<返回语句>" << endl;
#endif
    return 1;
}

/*
＜缺省＞   ::=  default :＜语句＞ 
*/
bool GrammaAna::default_check()
{
    if (NOWSYM != DEFAULTTK)
    {
        cout << "Error no default" << endl;
        return 0;
    }
    sym_index++;

    if (NOWSYM != COLON)
    {
        cout << "Error no ':' in default" << endl;
        return 0;
    }
    sym_index++;

    sentence_check(0);

    GRAOUT;
    cout << "<缺省>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<缺省>" << endl;
#endif
    return 1;
}

/*
＜情况表＞   ::=  ＜情况子语句＞{＜情况子语句＞} 
*/
bool GrammaAna::condition_table(int type)
{
    do
    {
        sub_condition(type);
    } while (NOWSYM == CASETK);

    GRAOUT;
    cout << "<情况表>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<情况表>" << endl;
#endif
    return 1;
}

/*
＜情况子语句＞  ::=  case＜常量＞：＜语句＞
*/
bool GrammaAna::sub_condition(int type)
{
    if (NOWSYM != CASETK)
    {
        cout << "Error in case" << endl;
        return 0;
    }
    sym_index++;

    int temtype = const_check() + 1;
    if (type != -1 and temtype != type)
    {
        add_error(LAS_LINENUM, TYPE_NOT_MATCH);
    }
    if (NOWSYM != COLON)
    {
        cout << "Error in case" << endl;
        return 0;
    }
    sym_index++;

    sentence_check(0);

    GRAOUT;
    cout << "<情况子语句>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<情况子语句>" << endl;
#endif
    return true;
}

/*
＜情况语句＞  ::=  switch ‘(’＜表达式＞‘)’ ‘{’＜情况表＞＜缺省＞‘}’ 
succ
*/
bool GrammaAna::switch_check()
{
    if (NOWSYM != SWITCHTK)
    {
        cout << "Error in switch" << endl;
        return 0;
    }
    sym_index++;
    if (NOWSYM != LPARENT)
    {
        cout << "Error in switch" << endl;
        return 0;
    }
    sym_index++;
    int typee;
    typee = expre_check();

    if (NOWSYM != RPARENT)
    {
        add_error(LAS_LINENUM, NO_RPARENT);
    }
    else
        sym_index++;

    if (NOWSYM != LBRACE)
    {
        cout << "Error in switch" << endl;
        return 0;
    }
    sym_index++;

    condition_table(typee);
    if (NOWSYM != DEFAULTTK)
    {
        add_error(NOWLINE, NO_DEFAULT);
    }
    else
        default_check();
    if (NOWSYM != RBRACE)
    {
        cout << "Error in switch" << endl;
        return 0;
    }
    sym_index++;

    GRAOUT;
    cout << "<情况语句>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<情况语句>" << endl;
#endif
    return 1;
}

/*
＜无返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')' 
*/
bool GrammaAna::callFuncNoRet_check()
{
    Symble_item tem_sym = Symble_item(NOWSTR, VAR, INT);
    auto id_sym = sym_table_stk[0].find(tem_sym); // 函数声明必定在全局
    //由于说了不会出现未定义的函数调用，所以这里就不检查了。
    if (NOWSYM != IDENFR)
    {
        cout << "Error in Func_call" << endl;
    }
    sym_index++;

    if (NOWSYM != LPARENT)
    {
        cout << "Error in Func_call no '(' " << endl;
    }
    sym_index++;
    vector<TYPE_NAME> tem = id_sym->para_list;
    value_para_list(tem);

    if (NOWSYM != RPARENT)
    {
        add_error(LAS_LINENUM, NO_RPARENT);
    }
    else
        sym_index++;

    GRAOUT;
    cout << "<无返回值函数调用语句>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<无返回值函数调用语句>" << endl;
#endif
    return 1;
}

/*
＜有返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'  
*/
int GrammaAna::callFuncRet_check()
{
    Symble_item tem_sym = Symble_item(NOWSTR, VAR, INT);
    auto id_sym = sym_table_stk[0].find(tem_sym); // 函数声明必定在全局
    //由于说了不会出现未定义的函数调用，所以这里就不检查了。
    if (NOWSYM != IDENFR)
    {
        cout << "Error in Func_call" << endl;
        return 0;
    }
    sym_index++;

    if (NOWSYM != LPARENT)
    {
        cout << "Error in Func_call no '(' " << endl;
        return 0;
    }
    sym_index++;
    vector<TYPE_NAME> tem = id_sym->para_list;
    value_para_list(tem);

    if (NOWSYM != RPARENT)
    {
        add_error(LAS_LINENUM, NO_RPARENT);
    }
    else
        sym_index++;
    GRAOUT;
    cout << "<有返回值函数调用语句>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<有返回值函数调用语句>" << endl;
#endif
    return id_sym->data_type == INT ? 1 : 2;
}

/*
＜值参数表＞   ::= ＜表达式＞{,＜表达式＞}｜＜空＞   
*/
bool GrammaAna::value_para_list(vector<TYPE_NAME> &real_para)
{
    //参数表为空的几种可能情况，由于可能缺少右括号，情况变复杂了
    if (NOWSYM == RPARENT or NOWSYM == SEMICN or NOWSYM == MULT or NOWSYM == DIV)
    {
        if (real_para.size() != 0)
        {
            add_error(LAS_LINENUM, PARA_NUM_NOT_MATCH);
        }
        GRAOUT;
        cout << "<值参数表>" << endl;
#ifndef ERROR_HANDLAR
        file_out << "<值参数表>" << endl;
#endif
        return 1;
    }
    int typee, para_num = 1, limit_num = real_para.size();
    typee = expre_check();
    if (para_num > limit_num)
    {
        add_error(LAS_LINENUM, PARA_NUM_NOT_MATCH);
    }
    else if (typee != -1 and typee != (real_para[para_num - 1] == INT ? 1 : 2))
    {
        add_error(LAS_LINENUM, PARA_TYPE_NOT_MATCH);
    }

    while (NOWSYM == COMMA)
    {
        sym_index++;
        typee = expre_check();
        para_num++;
        if (para_num > limit_num)
        {
            add_error(LAS_LINENUM, PARA_NUM_NOT_MATCH);
        }
        else if (typee != -1 and typee != (real_para[para_num - 1] == INT ? 1 : 2))
        {
            add_error(LAS_LINENUM, PARA_TYPE_NOT_MATCH);
        }
    }

    GRAOUT;
    cout << "<值参数表>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<值参数表>" << endl;
#endif
    return 1;
}

/*
＜赋值语句＞   ::=  
＜标识符＞＝＜表达式＞|
＜标识符＞'['＜表达式＞']'=＜表达式＞|
＜标识符＞'['＜表达式＞']''['＜表达式＞']' =＜表达式＞
*/
bool GrammaAna::assign_check()
{
#define ERROR_                                  \
    {                                           \
        cout << "Error in 赋值语句..." << endl; \
        return 0;                               \
    }

    if (NOWSYM != IDENFR)
        ERROR_
    sym_index++;

    IDENFR_EXIST_CONST_CHECK

    if (NOWSYM == ASSIGN) //变量
    {
        sym_index++;
        expre_check();
    }
    else if (NOWSYM == LBRACK)
    {
        sym_index++;
        int typee;
        typee = expre_check();
        if (typee == 2)
        {
            add_error(LAS_LINENUM, INDEX_ERR);
        }
        if (NOWSYM != RBRACK)
        {
            add_error(NOWLINE, NO_RBRACK);
        }
        else
            sym_index++;
        if (NOWSYM == ASSIGN) //一维数组
        {
            sym_index++;
            expre_check();
        }
        else if (NOWSYM == LBRACK) //二维数组
        {
            sym_index++;
            typee = expre_check();
            if (typee == 2)
            {
                add_error(LAS_LINENUM, INDEX_ERR);
            }
            if (NOWSYM != RBRACK)
            {
                add_error(NOWLINE, NO_RBRACK);
            }
            else
                sym_index++;
            if (NOWSYM != ASSIGN)
                ERROR_
            sym_index++;
            expre_check();
        }
    }
#undef ERROR_
    GRAOUT;
    cout << "<赋值语句>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<赋值语句>" << endl;
#endif
    return 1;
}

/*
    ＜语句＞    ::= 
    ＜循环语句＞｜              ok
    ＜条件语句＞|               ok
    
    ＜有返回值函数调用语句＞; /  ok
    ＜无返回值函数调用语句＞;｜  ok
    ＜赋值语句＞;｜             ok
    
    ＜读语句＞;｜               ok
    ＜写语句＞;｜               ok
    ＜情况语句＞｜              ok
    ＜空＞;|                    ok
    ＜返回语句＞; |             ok
     '{'＜语句列＞'}'           ok
*/
int GrammaAna::sentence_check(int flag)
{
    int retnum = 0;
    if (NOWSYM == FORTK || NOWSYM == WHILETK)
        whileFor_check();
    else if (NOWSYM == IFTK)
        if_check();
    else if (NOWSYM == SCANFTK)
    {
        scanf_check();
        if (NOWSYM != SEMICN)
        {
            add_error(LAS_LINENUM, NO_SEM);
        }
        else
            sym_index++;
    }
    else if (NOWSYM == PRINTFTK)
    {
        printf_check();
        if (NOWSYM != SEMICN)
        {
            add_error(LAS_LINENUM, NO_SEM);
        }
        else
            sym_index++;
    }
    else if (NOWSYM == SWITCHTK)
        switch_check();
    else if (NOWSYM == RETURNTK)
    {
        ret_check(flag);
        if (NOWSYM != SEMICN)
        {
            add_error(LAS_LINENUM, NO_SEM);
        }
        else
            sym_index++;
        retnum++;
    }
    else if (NOWSYM == LBRACE) //语句列
    {
        sym_index++;
        sentence_list(flag);
        if (NOWSYM != SEMICN)
        {
            add_error(LAS_LINENUM, NO_SEM);
        }
        else
            sym_index++;
    }
    else if (NOWSYM == SEMICN) //空语句
        sym_index++;
    else if (NOWSYM == IDENFR)
    {
        if (PEEKSYM(1) == LPARENT)
        {
            if (noRetFunc_symSet.find(NOWSTR) != noRetFunc_symSet.end())
                callFuncNoRet_check();
            else
                callFuncRet_check();
        }
        else
            assign_check();
        if (NOWSYM != SEMICN)
        {
            add_error(LAS_LINENUM, NO_SEM);
        }
        else
            sym_index++;
    }
    else
    {
        add_error(LAS_LINENUM, NO_SEM);
    }
    GRAOUT;
    cout << "<语句>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<语句>" << endl;
#endif
    return retnum;
}

/*
    ＜语句列＞   ::= ｛＜语句＞｝
*/
int GrammaAna::sentence_list(int flag)
{
    int has_ret = 0;
    while (NOWSYM != RBRACE)
        has_ret += sentence_check(flag); // 只要不是'}'就往下读，不知道对不对
    GRAOUT;
    cout << "<语句列>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<语句列>" << endl;
#endif
    return has_ret;
}

/*
    ＜复合语句＞ ::=［＜常量说明＞］［＜变量说明＞］＜语句列＞
*/
int GrammaAna::mult_sentence(int flag)
{
    if (NOWSYM == CONSTTK)
        const_stat();
    if (NOWSYM == INTTK || NOWSYM == CHARTK)
        var_statment();
    int ret = sentence_list(flag);
    GRAOUT;
    cout << "<复合语句>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<复合语句>" << endl;
#endif
    return ret;
}

/*
    ＜无返回值函数定义＞  ::= void＜标识符＞'('＜参数表＞')''{'＜复合语句＞'}'  
    这里可能的错误是：少了')'
*/
bool GrammaAna::func_noRet_define()
{
    ret_in_func = 0;
    need_ret_type = 0;
    if (NOWSYM != VOIDTK)
    {
        cout << "Error in func func_noRet_def" << endl;
        return 0;
    }
    sym_index++;
    if (NOWSYM != IDENFR)
    {
        cout << "Error in func func_noRet_def" << endl;
        return 0;
    }
    Symble_item tem_sym = Symble_item(NOWSTR, FUNC_NO_RET, VOID);
    noRetFunc_symSet.insert(NOWSTR);
    sym_index++;
    if (NOWSYM != LPARENT)
    {
        cout << "Error in func func_noRet_def" << endl;
        return 0;
    }
    sym_index++;

    NEW_SYMSTK;

    para_list(tem_sym); // 参数表
    if (NOWSYM != RPARENT)
    {
        cout << "Error in func func_noRet_def" << endl;
        add_error(LAS_LINENUM, NO_RPARENT);
    }
    else
        sym_index++;
    //这里因为读取的参数要加入到符号表里面，所以在把保存函数的符号项加到全局符号表里时，先new一个
    //符号表，所以在这里面插入的时候要选择倒数第二个符号表即全局符号表
    if (sym_table_stk[sym_table_stk.size() - 2].find(tem_sym) != sym_table_stk[sym_table_stk.size() - 2].end())
    {
        cout << "redef" << endl;
        add_error(LAS_LINENUM, NAME_REDEF);
    }
    else
        sym_table_stk[sym_table_stk.size() - 2].insert(tem_sym);

    if (NOWSYM != LBRACE)
    {
        cout << "Error in func func_noRet_def" << endl;
        return 0;
    }
    sym_index++;
    mult_sentence(0); // 复合语句
    if (NOWSYM != RBRACE)
    {
        cout << "Error in func func_noRet_def" << endl;
        return 0;
    }
    sym_index++;

    POP_SYMSTK;

    GRAOUT;
    cout << "<无返回值函数定义>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<无返回值函数定义>" << endl;
#endif
    return true;
}

/*
＜声明头部＞   ::=  int＜标识符＞ |char＜标识符＞
*/
bool GrammaAna::statment_head()
{
    if (NOWSYM != INTTK and NOWSYM != CHARTK)
    {
        cout << "Error in statment_head" << endl;
        return 0;
    }
    sym_index++;
    if (NOWSYM != IDENFR)
    {
        cout << "Error in statment_head" << endl;
        return 0;
    }
    sym_index++;

    GRAOUT;
    cout << "<声明头部>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<声明头部>" << endl;
#endif
    return true;
}

/*
＜有返回值函数定义＞  ::=  ＜声明头部＞'('＜参数表＞')' '{'＜复合语句＞'}' 
*/
bool GrammaAna::func_define()
{
    ret_in_func = 0;
    need_ret_type = 0;
    int typee = NOWSYM == INTTK ? 1 : 2;
    need_ret_type = typee;
    sym_index++;
    Symble_item tem_sym = Symble_item(NOWSTR, FUNC, typee == 1 ? INT : CHAR);
    sym_index--;
    statment_head();

    if (NOWSYM != LPARENT)
    {
        cout << "Error in func func_def" << endl;
        return 0;
    }
    sym_index++;
    NEW_SYMSTK;

    para_list(tem_sym); // 参数表
    if (NOWSYM != RPARENT)
    {
        cout << "Error in func func_def" << endl;
        add_error(LAS_LINENUM, NO_RPARENT);
    }
    else
        sym_index++;
    //这里因为读取的参数要加入到符号表里面，所以在把保存函数的符号项加到全局符号表里时，先new一个
    //符号表，所以在这里面插入的时候要选择倒数第二个符号表即全局符号表
    if (sym_table_stk[sym_table_stk.size() - 2].find(tem_sym) != sym_table_stk[sym_table_stk.size() - 2].end())
    {
        cout << "redef" << endl;
        add_error(LAS_LINENUM, NAME_REDEF);
    }
    else
        sym_table_stk[sym_table_stk.size() - 2].insert(tem_sym);
    if (NOWSYM != LBRACE)
    {
        cout << "Error in func func_def" << endl;
        return 0;
    }
    sym_index++;
    mult_sentence(0); // 复合语句
    if (NOWSYM != RBRACE)
    {
        cout << "Error in func func_def" << endl;
        return 0;
    }
    sym_index++;
    POP_SYMSTK;

    if (ret_in_func == 0)
    {
        add_error(NOWLINE, RET_FUNC_ERR);
    }

    GRAOUT;
    cout << "<有返回值函数定义>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<有返回值函数定义>" << endl;
#endif
    return true;
}

/*
＜主函数＞    ::= void main‘(’‘)’ ‘{’＜复合语句＞‘}’ 
*/
bool GrammaAna::main_define()
{
#define ERROR_                                \
    {                                         \
        cout << "Error in main_func" << endl; \
        return 0;                             \
    }
    ret_in_func = 0;
    need_ret_type = 0;
    if (NOWSYM != VOIDTK)
        ERROR_
    sym_index++;
    if (NOWSYM != MAINTK)
        ERROR_
    sym_index++;
    if (NOWSYM != LPARENT) // (
        ERROR_
    sym_index++;
    if (NOWSYM != RPARENT) // )
    {
        add_error(NOWLINE, NO_RPARENT);
    }
    sym_index++;

    NEW_SYMSTK;

    if (NOWSYM != LBRACE) // {
        ERROR_
    sym_index++;

    mult_sentence(0);

    if (NOWSYM != RBRACE) // {
        ERROR_
    sym_index++;
#undef ERROR_

    POP_SYMSTK;

    GRAOUT;
    cout << "<主函数>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<主函数>" << endl;
#endif
    return 1;
}

/*
＜程序＞    ::= ［＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞
*/
bool GrammaAna::top_programe()
{
    NEW_SYMSTK;
    //常量定义
    if (NOWSYM == CONSTTK)
        if (!const_stat())
        {
            cout << "Error in func top_program" << endl;
            return false;
        }
    //变量定义
    //判断是否是函数，正常的函数第三个sym必为括号
    if (lexRes[sym_index + 2].first != LPARENT) //'(' //未判断越界
        if (!var_statment())
        {
            //Error
            cout << "Error in func top_program" << endl;
        }
    //func
    while (true)
    {
        if (NOWSYM == VOIDTK)
        {
            if (lexRes[sym_index + 1].first == MAINTK) //maybe error 未判断越界
                break;
            else
                func_noRet_define();
        }
        if (NOWSYM == INTTK || NOWSYM == CHARTK)
            func_define();
        else
        {
            //ERROR
            cout << "Error in func top_program" << endl;
        }
    }
    main_define();

    POP_SYMSTK;

    GRAOUT;
    cout << "<程序>" << endl;
#ifndef ERROR_HANDLAR
    file_out << "<程序>" << endl;
#endif
    return 1;
}

void GrammaAna::startGramAna()
{
    lex_ana.startAna();
    top_programe();
    if (NOWSYM != EOFTK)
    {
        cout << "出现了神秘错误，程序main函数后面不为EOF" << endl;
    }
    lex_ana.errHandle.OUTALL();
}

void GrammaAna::add_error(int line_num, ERROR_TYPE e)
{
    lex_ana.errHandle.add(line_num, e);
}
