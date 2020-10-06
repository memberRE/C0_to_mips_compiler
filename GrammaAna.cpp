#include "GrammaAna.h"
#include <map>
#include <set>
#include <vector>
#include <wchar.h>
using namespace std;
extern symType now_type;
extern string str_read;
extern string lower_str_read;
extern int line_num; // line number
extern map<string, symType> str2type;
extern map<symType, string> type2str;
extern vector<pair<symType, string>> lexRes;

vector<symType> sym_stk;
set<string> noRetFunc_symSet;
int sym_top = 0, sym_index = 0; //sym_index指向第一个未处理的sym
int out_index = 0;              //out_index指向下一个需要输出的sym

#define NOWSYM (sym_index < lexRes.size() ? lexRes[sym_index].first : EOFTK)
#define PEEKSYM(x) (sym_index + x < lexRes.size() ? lexRes[sym_index + x].first : EOFTK)
#define NOWSTR lexRes[sym_index].second

#define PUSHSTK                          \
    {                                    \
        sym_top++;                       \
        if (sym_top >= sym_stk.size())   \
            sym_stk.push_back(now_type); \
    }

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

void main_define()
{
    // 确保当前sym为main
}

/*
    ＜整数＞        ::= ［＋｜－］＜无符号整数＞
*/
bool GrammaAna::integer_check()
{
    if (NOWSYM == PLUS or NOWSYM == MINU)
        sym_index++;
    if (NOWSYM == INTCON)
    {
        sym_index++;
        GRAOUT;
        file_out << "<无符号整数>" << endl;
        cout << "<无符号整数>" << endl;
    }

    else
    {
        //Error
        cout << "Error in func integer_check" << endl;
        return false;
    }
    GRAOUT;
    file_out << "<整数>" << endl;
    cout << "<整数>" << endl;
    return true;
}

/*
＜常量定义＞::=int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}  | char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞} 
*/
bool GrammaAna::const_def()
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
        sym_index++;
        if (NOWSYM != IDENFR)
        {
            //Error
            cout << "Error in func const_def2" << endl;
            return false;
        }
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
            if (!integer_check())
            {
                //Error
                cout << "Error in func const_def4" << endl;
                return false;
            }
        }
        else
        {
            // if (NOWSYM != SIGQUO)
            // {
            //     //Error
            //     cout << "Error in func const_def5 缺少单引号" << endl;
            //     return false;
            // }
            // sym_index++;
            if (NOWSYM != CHARCON)
            {
                //Error
                cout << "Error in func const_def6" << endl;
                return false;
            }
            sym_index++;
        }
    } while (NOWSYM == COMMA);
    GRAOUT;
    file_out << "<常量定义>" << endl;
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
            cout << "Error in func const_stat" << endl;
        }
        sym_index++;
    } while (1);
    GRAOUT;
    cout << "<常量说明>" << endl;
    file_out << "<常量说明>" << endl;
    return true;
}

/*
＜常量＞   ::=  ＜整数＞|＜字符＞
返回值： 1字符；2整数
*/
int GrammaAna::const_check()
{
    if (NOWSYM == CHARCON)
    {
        sym_index++;
        GRAOUT;
        cout << "<常量>" << endl;
        file_out << "<常量>" << endl;
        return 1;
    }
    else
    {
        integer_check();
        GRAOUT;
        cout << "<常量>" << endl;
        file_out << "<常量>" << endl;
        return 2;
    }
}

/*
    ＜变量定义及初始化＞  ::= ＜类型标识符＞＜标识符＞=＜常量＞ |
    ＜类型标识符＞＜标识符＞'['＜无符号整数＞']'='{'＜常量＞{,＜常量＞}'}' |
    ＜类型标识符＞＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']'='{''{'＜常量＞{,＜常量＞}'}'{, '{'＜常量＞{,＜常量＞}'}'}'}'
*/
bool GrammaAna::var_init_def(int firType)
{
    if (firType == 1) //变量
        const_check();
    else if (firType == 2) //一维数组
    {
        if (NOWSYM != LBRACE) //{
        {
            cout << "Error in func var_init_def" << endl;
            return 0;
        }
        do
        {
            sym_index++;
            const_check();
        } while (NOWSYM == COMMA);
        if (NOWSYM != RBRACE) //{
        {
            cout << "Error in func var_init_def" << endl;
            return 0;
        }
        sym_index++;
    }
    else if (firType == 3)
    {
        if (NOWSYM != LBRACE) //{
        {
            cout << "Error in func var_init_def" << endl;
            return 0;
        }
        do
        {
            sym_index++;
            if (NOWSYM != LBRACE) //{
            {
                cout << "Error in func var_init_def" << endl;
                return 0;
            }
            do
            {
                sym_index++;
                const_check();
            } while (NOWSYM == COMMA);
            if (NOWSYM != RBRACE) //}
            {
                cout << "Error in func var_init_def" << endl;
                return 0;
            }
            sym_index++;
        } while (NOWSYM == COMMA);
        if (NOWSYM != RBRACE) //{
        {
            cout << "Error in func var_init_def" << endl;
            return 0;
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
    file_out << "<变量定义及初始化>" << endl;
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

bool GrammaAna::var_noInit_def(int firType)
{
    while (NOWSYM == COMMA)
    {
        sym_index++;
        if (!array_check())
            return 0;
    }
    GRAOUT;
    cout << "<变量定义无初始化>" << endl;
    file_out << "<变量定义无初始化>" << endl;
    return 1;
}

int GrammaAna::array_check()
{
    if (NOWSYM == IDENFR and PEEKSYM(1) == LBRACK and PEEKSYM(4) == LBRACK) // 二维数组 a[x][y]
    {
        sym_index += 2;
        if (NOWSYM != INTCON)
        {
            cout << "Error in func array_check" << endl;
            return 0;
        }
        sym_index++;
        GRAOUT;
        cout << "<无符号整数>" << endl;
        file_out << "<无符号整数>" << endl;
        sym_index += 2;
        if (NOWSYM != INTCON)
        {
            cout << "Error in func array_check" << endl;
            return 0;
        }
        sym_index++;
        GRAOUT;
        cout << "<无符号整数>" << endl;
        file_out << "<无符号整数>" << endl;
        sym_index++;
        return 3;
    }
    else if (NOWSYM == IDENFR and PEEKSYM(1) == LBRACK) //一维数组
    {
        sym_index += 2;
        if (NOWSYM != INTCON)
        {
            cout << "Error in func array_check" << endl;
            return 0;
        }
        sym_index++;
        GRAOUT;
        cout << "<无符号整数>" << endl;
        file_out << "<无符号整数>" << endl;
        sym_index++;
        return 2;
    }
    else if (NOWSYM == IDENFR) // 变量
    {
        sym_index++;
        return 1;
    }
    else
    {
        //Error
        cout << "Error in func array_check" << endl;
        return 0;
    }
}
/*
    ＜变量定义＞ ::= ＜变量定义无初始化＞|＜变量定义及初始化＞
*/
bool GrammaAna::var_define()
{
    if (NOWSYM != INTTK and NOWSYM != CHARTK)
    {
        cout << "Error in func var_define" << endl;
        return false;
    }
    sym_index++; //现在指向标识符
    int type = array_check();
    if (type == 0)
        return false;
    if (NOWSYM == ASSIGN)
    {
        sym_index++;
        if (!var_init_def(type))
        {
            cout << "Error in func var_define" << endl;
            return 0;
        }
    }
    else if (!var_noInit_def(type))
    {
        cout << "Error in func var_define" << endl;
        return 0;
    }
    GRAOUT;
    cout << "<变量定义>" << endl;
    file_out << "<变量定义>" << endl;
    return true;
}
/*
＜变量说明＞  ::= ＜变量定义＞;{＜变量定义＞;}
*/
bool GrammaAna::var_statment()
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
            cout << "Error in func var_statment" << endl;
            return false;
        }
        sym_index++;
    } while (lexRes[sym_index + 2].first != LPARENT and (NOWSYM == INTTK or NOWSYM == CHARTK) and PEEKSYM(1) == IDENFR);
    GRAOUT;
    cout << "<变量说明>" << endl;
    file_out << "<变量说明>" << endl;
    return true;
}
/*
    ＜参数表＞    ::=  ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}| ＜空＞
*/
bool GrammaAna::para_list()
{
    if (NOWSYM == RPARENT)
    {
        GRAOUT;
        cout << "<参数表>" << endl;
        file_out << "<参数表>" << endl;
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
        sym_index++;
        if (NOWSYM != IDENFR)
        {
            cout << "Error in func para_list" << endl;
            return 0;
        }
        sym_index++;
    } while (NOWSYM == COMMA);
    GRAOUT;
    cout << "<参数表>" << endl;
    file_out << "<参数表>" << endl;
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
    file_out << "<无符号整数>" << endl;
    GRAOUT;
    cout << "<步长>" << endl;
    file_out << "<步长>" << endl;
    return 1;
}

/*
＜条件＞    ::=  ＜表达式＞＜关系运算符＞＜表达式＞
*/
bool GrammaAna::condition_check()
{
    expre_check();
    if (!(NOWSYM == LSS or NOWSYM == LEQ or NOWSYM == GRE or NOWSYM == GEQ or NOWSYM == EQL or NOWSYM == NEQ))
    {
        cout << "Error in func condition_check" << endl;
        return 0;
    }
    sym_index++;
    expre_check();
    GRAOUT;
    cout << "<条件>" << endl;
    file_out << "<条件>" << endl;
    return 1;
}

/*
＜表达式＞::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}  
*/
bool GrammaAna::expre_check()
{
    if (NOWSYM == PLUS || NOWSYM == MINU)
        sym_index++;
    item_check();
    while (NOWSYM == PLUS || NOWSYM == MINU) //fuck！！！ 加法运算符居然包括减法
    {
        sym_index++;
        item_check();
    }
    GRAOUT;
    cout << "<表达式>" << endl;
    file_out << "<表达式>" << endl;
    return 1;
}

/*
＜项＞::= ＜因子＞{＜乘法运算符＞＜因子＞}
*/
bool GrammaAna::item_check()
{
    factor_check();
    while (NOWSYM == MULT || NOWSYM == DIV)
    {
        sym_index++;
        factor_check();
    }
    GRAOUT;
    cout << "<项>" << endl;
    file_out << "<项>" << endl;
    return 1;
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
bool GrammaAna::factor_check()
{
    if (NOWSYM == LPARENT) // '('
    {
        sym_index++;
        expre_check();
        if (NOWSYM != RPARENT) // )
        {
            cout << "Error in factor_check" << endl;
            return 0;
        }
        sym_index++;
    }
    else if (NOWSYM == CHARCON)
    {
        sym_index++;
    }
    else if (NOWSYM == IDENFR and PEEKSYM(1) != LPARENT)
    {
        sym_index++;
        if (NOWSYM == LBRACK) // [
        {
            sym_index++;
            expre_check();
            if (NOWSYM != RBRACK)
            {
                cout << "Error in factor_check" << endl;
                return 0;
            }
            sym_index++;
            if (NOWSYM == LBRACK)
            {
                sym_index++;
                expre_check();
                if (NOWSYM != RBRACK)
                {
                    cout << "Error in factor_check" << endl;
                    return 0;
                }
                sym_index++;
            }
        }   
    }
    else if (NOWSYM == IDENFR and PEEKSYM(1) == LPARENT)
        callFuncRet_check();
    else
        integer_check();
    GRAOUT;
    cout << "<因子>" << endl;
    file_out << "<因子>" << endl;
    return 1;
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
            ERROR_
        sym_index++;
        sentence_check();
    }
    else if (NOWSYM == FORTK)
    {
        sym_index++;
        if (NOWSYM != LPARENT)
            ERROR_
        sym_index++;
        if (NOWSYM != IDENFR)
            ERROR_
        sym_index++;
        if (NOWSYM != ASSIGN)
            ERROR_
        sym_index++;

        expre_check();

        if (NOWSYM != SEMICN)
            ERROR_
        sym_index++;

        condition_check();

        if (NOWSYM != SEMICN)
            ERROR_
        sym_index++;
        if (NOWSYM != IDENFR)
            ERROR_
        sym_index++;
        if (NOWSYM != ASSIGN)
            ERROR_
        sym_index++;
        if (NOWSYM != IDENFR)
            ERROR_
        sym_index++;
        if (NOWSYM != PLUS and NOWSYM != MINU)
            ERROR_
        sym_index++;

        step_check();

        if (NOWSYM != RPARENT)
            ERROR_
        sym_index++;

        sentence_check();
    }
    else
        ERROR_
    GRAOUT;
    cout << "<循环语句>" << endl;
    file_out << "<循环语句>" << endl;
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
        cout << "Error in ifcheck" << endl;
        return 0;
    }
    sym_index++;

    sentence_check();

    if (NOWSYM == ELSETK)
    {
        sym_index++;
        sentence_check();
    }

    GRAOUT
    cout << "<条件语句>" << endl;
    file_out << "<条件语句>" << endl;
    return 1;
}

/*
＜读语句＞    ::=  scanf '('＜标识符＞')' 
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
    sym_index++;
    if (NOWSYM != RPARENT)
    {
        cout << "Error in scanf" << endl;
        return 0;
    }
    sym_index++;

    GRAOUT;
    cout << "<读语句>" << endl;
    file_out << "<读语句>" << endl;
    return 1;
}

/*
＜写语句＞    ::= 
printf '(' ＜字符串＞,＜表达式＞ ')'|
printf '('＜字符串＞ ')'|
printf '('＜表达式＞')' 
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
        file_out << "<字符串>" << endl;
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
        cout << "Error in scanf" << endl;
        return 0;
    }
    sym_index++;

    GRAOUT;
    cout << "<写语句>" << endl;
    file_out << "<写语句>" << endl;
    return 1;
}

/*
＜返回语句＞   ::=  return['('＜表达式＞')']   
*/
bool GrammaAna::ret_check()
{
    if (NOWSYM != RETURNTK)
    {
        cout << "Error in return" << endl;
        return 0;
    }
    sym_index++;
    if (NOWSYM == LPARENT)
    {
        sym_index++;
        expre_check();
        if (NOWSYM != RPARENT)
        {
            cout << "Error in return" << endl;
            return 0;
        }
        sym_index++;
    }
    GRAOUT;
    cout << "<返回语句>" << endl;
    file_out << "<返回语句>" << endl;
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

    sentence_check();

    GRAOUT;
    cout << "<缺省>" << endl;
    file_out << "<缺省>" << endl;
    return 1;
}

/*
＜情况表＞   ::=  ＜情况子语句＞{＜情况子语句＞} 
*/
bool GrammaAna::condition_table()
{
    do
    {
        sub_condition();
    } while (NOWSYM == CASETK);

    GRAOUT;
    cout << "<情况表>" << endl;
    file_out << "<情况表>" << endl;
    return 1;
}

/*
＜情况子语句＞  ::=  case＜常量＞：＜语句＞
*/
bool GrammaAna::sub_condition()
{
    if (NOWSYM != CASETK)
    {
        cout << "Error in case" << endl;
        return 0;
    }
    sym_index++;

    const_check();

    if (NOWSYM != COLON)
    {
        cout << "Error in case" << endl;
        return 0;
    }
    sym_index++;

    sentence_check();

    GRAOUT;
    cout << "<情况子语句>" << endl;
    file_out << "<情况子语句>" << endl;
    return true;
}

/*
＜情况语句＞  ::=  switch ‘(’＜表达式＞‘)’ ‘{’＜情况表＞＜缺省＞‘}’ 
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

    expre_check();

    if (NOWSYM != RPARENT)
    {
        cout << "Error in switch" << endl;
        return 0;
    }
    sym_index++;

    if (NOWSYM != LBRACE)
    {
        cout << "Error in switch" << endl;
        return 0;
    }
    sym_index++;

    condition_table();

    default_check();

    if (NOWSYM != RBRACE)
    {
        cout << "Error in switch" << endl;
        return 0;
    }
    sym_index++;

    GRAOUT;
    cout << "<情况语句>" << endl;
    file_out << "<情况语句>" << endl;
    return 1;
}

/*
＜无返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')' 
*/
bool GrammaAna::callFuncNoRet_check()
{
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

    value_para_list();

    if (NOWSYM != RPARENT)
    {
        cout << "Error in Func_call no '(' " << endl;
        return 0;
    }
    sym_index++;

    GRAOUT;
    cout << "<无返回值函数调用语句>" << endl;
    file_out << "<无返回值函数调用语句>" << endl;
    return 1;
}

/*
＜有返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'  
*/
bool GrammaAna::callFuncRet_check()
{
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

    value_para_list();

    if (NOWSYM != RPARENT)
    {
        cout << "Error in Func_call no '(' " << endl;
        return 0;
    }
    sym_index++;

    GRAOUT;
    cout << "<有返回值函数调用语句>" << endl;
    file_out << "<有返回值函数调用语句>" << endl;
    return 1;
}

/*
＜值参数表＞   ::= ＜表达式＞{,＜表达式＞}｜＜空＞   
*/
bool GrammaAna::value_para_list()
{
    if (NOWSYM == RPARENT) //如果是右括号，说明参数表为空
    {
        GRAOUT;
        cout << "<值参数表>" << endl;
        file_out << "<值参数表>" << endl;
        return 1;
    }

    expre_check();
    while (NOWSYM == COMMA)
    {
        sym_index++;
        expre_check();
    }

    GRAOUT;
    cout << "<值参数表>" << endl;
    file_out << "<值参数表>" << endl;
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
#define ERROR_                               \
    {                                        \
        cout << "Error in 赋值语句..." << endl; \
        return 0;                            \
    }

    if (NOWSYM != IDENFR)
        ERROR_
    sym_index++;

    if (NOWSYM == ASSIGN) //变量
    {
        sym_index++;
        expre_check();
    }
    else if (NOWSYM == LBRACK)
    {
        sym_index++;
        expre_check();
        if (NOWSYM != RBRACK)
            ERROR_
        sym_index++;
        if (NOWSYM == ASSIGN) //一维数组
        {
            sym_index++;
            expre_check();
        }
        else if (NOWSYM == LBRACK) //二维数组
        {
            sym_index++;
            expre_check();
            if (NOWSYM != RBRACK)
                ERROR_
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
    file_out << "<赋值语句>" << endl;
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
bool GrammaAna::sentence_check()
{
    if (NOWSYM == FORTK || NOWSYM == WHILETK)
        whileFor_check();
    else if (NOWSYM == IFTK)
        if_check();
    else if (NOWSYM == SCANFTK)
    {
        scanf_check();
        if (NOWSYM != SEMICN)
        {
            cout << "Error in ';'" << endl;
            return 0;
        }
        sym_index++;
    }
    else if (NOWSYM == PRINTFTK)
    {
        printf_check();
        if (NOWSYM != SEMICN)
        {
            cout << "Error in ';'" << endl;
            return 0;
        }
        sym_index++;
    }
    else if (NOWSYM == SWITCHTK)
        switch_check();
    else if (NOWSYM == RETURNTK)
    {
        ret_check();
        if (NOWSYM != SEMICN)
        {
            cout << "Error in ';'" << endl;
            return 0;
        }
        sym_index++;
    }
    else if (NOWSYM == LBRACE) //语句列
    {
        sym_index++;
        sentence_list();
        if (NOWSYM != RBRACE)
        {
            cout << "Error in func sectence_check" << endl;
            return 0;
        }
        sym_index++;
    }
    else if (NOWSYM == SEMICN) //空语句
        sym_index++;
    else
    {
        if (NOWSYM != IDENFR)
        {
            cout << "Error in func sentence_check" << endl;
            return 0;
        }
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
            cout << "Error in ';'" << endl;
            return 0;
        }
        sym_index++;
    }
    GRAOUT;
    cout << "<语句>" << endl;
    file_out << "<语句>" << endl;
    return 1;
}

/*
    ＜语句列＞   ::= ｛＜语句＞｝
*/
bool GrammaAna::sentence_list()
{
    while (NOWSYM != RBRACE)
        sentence_check(); // 只要不是'}'就往下读，不知道对不对
    GRAOUT;
    cout << "<语句列>" << endl;
    file_out << "<语句列>" << endl;
    return true;
}

/*
    ＜复合语句＞ ::=［＜常量说明＞］［＜变量说明＞］＜语句列＞
*/
bool GrammaAna::mult_sentence()
{
    if (NOWSYM == CONSTTK)
        const_stat();
    if (NOWSYM == INTTK || NOWSYM == CHARTK)
        var_statment();
    sentence_list();
    GRAOUT;
    cout << "<复合语句>" << endl;
    file_out << "<复合语句>" << endl;
    return 1;
}

/*
    ＜无返回值函数定义＞  ::= void＜标识符＞'('＜参数表＞')''{'＜复合语句＞'}'  
*/
bool GrammaAna::func_noRet_define()
{
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
    noRetFunc_symSet.insert(NOWSTR);
    sym_index++;
    if (NOWSYM != LPARENT)
    {
        cout << "Error in func func_noRet_def" << endl;
        return 0;
    }
    sym_index++;
    para_list(); // 参数表
    if (NOWSYM != RPARENT)
    {
        cout << "Error in func func_noRet_def" << endl;
        return 0;
    }
    sym_index++;
    if (NOWSYM != LBRACE)
    {
        cout << "Error in func func_noRet_def" << endl;
        return 0;
    }
    sym_index++;
    mult_sentence(); // 复合语句
    if (NOWSYM != RBRACE)
    {
        cout << "Error in func func_noRet_def" << endl;
        return 0;
    }
    sym_index++;
    GRAOUT;
    cout << "<无返回值函数定义>" << endl;
    file_out << "<无返回值函数定义>" << endl;
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
    file_out << "<声明头部>" << endl;
    return true;
}

/*
＜有返回值函数定义＞  ::=  ＜声明头部＞'('＜参数表＞')' '{'＜复合语句＞'}' 
*/
bool GrammaAna::func_define()
{
    statment_head();

    if (NOWSYM != LPARENT)
    {
        cout << "Error in func func_def" << endl;
        return 0;
    }
    sym_index++;
    para_list(); // 参数表
    if (NOWSYM != RPARENT)
    {
        cout << "Error in func func_def" << endl;
        return 0;
    }
    sym_index++;
    if (NOWSYM != LBRACE)
    {
        cout << "Error in func func_def" << endl;
        return 0;
    }
    sym_index++;
    mult_sentence(); // 复合语句
    if (NOWSYM != RBRACE)
    {
        cout << "Error in func func_def" << endl;
        return 0;
    }
    sym_index++;
    GRAOUT;
    cout << "<有返回值函数定义>" << endl;
    file_out << "<有返回值函数定义>" << endl;
    return true;
}

/*
＜主函数＞    ::= void main‘(’‘)’ ‘{’＜复合语句＞‘}’ 
*/
bool GrammaAna::main_define()
{
#define ERROR_                               \
    {                                        \
        cout << "Error in main_func" << endl; \
        return 0;                            \
    }

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
        ERROR_
    sym_index++;
    if (NOWSYM != LBRACE) // {
        ERROR_
    sym_index++;

    mult_sentence();

    if (NOWSYM != RBRACE) // {
        ERROR_
    sym_index++;
#undef ERROR_
    GRAOUT;
    cout << "<主函数>" << endl;
    file_out << "<主函数>" << endl;
    return 1;
}

/*
＜程序＞    ::= ［＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞
*/
bool GrammaAna::top_programe()
{
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
    GRAOUT;
    cout << "<程序>" << endl;
    file_out << "<程序>" << endl;
    return 1;
}

void GrammaAna::startGramAna()
{
    lex_ana.startAna();
    top_programe();
    if (NOWSYM != EOFTK)
    {
        cout<<"出现了神秘错误，程序main函数后面不为EOF"<<endl;
    }
}
