#include "LexAla.h"
#include <algorithm>
#include <cctype>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <map>
#include <stdio.h>
#include <string>
#include <vector>
#define DEBUG
//#define Lexout
using namespace std;
//static char now_ch = ' ';
int integer_read;
string str_read = "";
string lower_str_read = "";
int line_num = 1; // line number
symType now_type;
map<string, symType> str2type;
map<symType, string> type2str;
vector<pair<symType, string>> lexRes;

ostream &operator<<(ostream &ofs, symType st)
{
    ofs << type2str[st];
    return ofs;
}
void LexAnalyzer::get_sym()
{
    //now_ch = file_in.get();
    // ensure now_ch has been written
    while (isspace(now_ch))
    {
        if (now_ch == '\n')
            line_num++;
        now_ch = file_in.get();
    }
    if (isdigit(now_ch))
    {
        now_type = INTCON;
        str_read.clear();
        lower_str_read.clear();
        integer_read = now_ch - '0';
        str_read.push_back(now_ch);
        while (isdigit(now_ch = file_in.get()))
        {
            integer_read = integer_read * 10 + now_ch - '0';
            str_read.push_back(now_ch);
        }
    }
    else if (isalpha(now_ch) || now_ch == '_')
    {
        str_read.clear();
        lower_str_read.clear();
        str_read.push_back(now_ch);
        lower_str_read.push_back(tolower(now_ch));
        while (isalnum(now_ch = file_in.get()) || now_ch == '_')
            str_read.push_back(now_ch), lower_str_read.push_back(tolower(now_ch));
        auto it = str2type.find(lower_str_read);
        if (it == str2type.end())
            now_type = IDENFR;
        else
            now_type = it->second;
    }
    else
    {
        str_read.clear();
        lower_str_read.clear();
        str_read.push_back(now_ch);
        char tem = 0;
        switch (now_ch)
        {
        case '+':
            now_type = PLUS;
            break;
        case '-':
            now_type = MINU;
            break;
        case '/':
            now_type = DIV;
            break;
        case '*':
            now_type = MULT;
            break;
        case '<':
            tem = file_in.peek();
            now_type = tem == '=' ? LEQ : LSS;
            break;
        case '>':
            tem = file_in.peek();
            now_type = tem == '=' ? GEQ : GRE;
            break;
        case '=':
            tem = file_in.peek();
            now_type = tem == '=' ? EQL : ASSIGN;
            break;
        case '!':
            tem = file_in.peek();
            now_type = tem == '=' ? NEQ : ILLEGAL;
            break;
        case ':':
            now_type = COLON;
            break;
        case ';':
            now_type = SEMICN;
            break;
        case ',':
            now_type = COMMA;
            break;
        case '(':
            now_type = LPARENT;
            break;
        case ')':
            now_type = RPARENT;
            break;
        case '[':
            now_type = LBRACK;
            break;
        case ']':
            now_type = RBRACK;
            break;
        case '{':
            now_type = LBRACE;
            break;
        case '}':
            now_type = RBRACE;
            break;
        case EOF:
            now_type = EOFTK;
            break;
        default:
            now_type = ILLEGAL;
        }
        if (tem == '=')
            str_read.push_back('='), file_in.get();
        now_ch = file_in.get();
    }
}

void LexAnalyzer::get_nex_char()
{
    str_read.clear();
    str_read.push_back(now_ch);
    now_ch = file_in.get();
    now_type = CHARCON;
    //跳过下一个单引号
    now_ch = file_in.get();
}

void LexAnalyzer::get_nex_string()
{
    str_read.clear();
    str_read.push_back(now_ch);
    now_type = STRCON;
    while ((now_ch = file_in.get()) != '\"')
    {
        if (now_ch == EOF) //貌似没必要
            break;
        str_read.push_back(now_ch);
    }
    now_ch = file_in.get(); //跳过下一个双引号
}

void LexAnalyzer::initStr2Type()
{
    str2type.insert({"const", CONSTTK});
    type2str.insert({CONSTTK, "CONSTTK"});
    str2type.insert({"int", INTTK});
    type2str.insert({INTTK, "INTTK"});
    str2type.insert({"char", CHARTK});
    type2str.insert({CHARTK, "CHARTK"});
    str2type.insert({"void", VOIDTK});
    type2str.insert({VOIDTK, "VOIDTK"});
    str2type.insert({"main", MAINTK});
    type2str.insert({MAINTK, "MAINTK"});
    str2type.insert({"if", IFTK});
    type2str.insert({IFTK, "IFTK"});
    str2type.insert({"else", ELSETK});
    type2str.insert({ELSETK, "ELSETK"});
    str2type.insert({"switch", SWITCHTK});
    type2str.insert({SWITCHTK, "SWITCHTK"});
    str2type.insert({"case", CASETK});
    type2str.insert({CASETK, "CASETK"});
    str2type.insert({"default", DEFAULTTK});
    type2str.insert({DEFAULTTK, "DEFAULTTK"});
    str2type.insert({"while", WHILETK});
    type2str.insert({WHILETK, "WHILETK"});
    str2type.insert({"for", FORTK});
    type2str.insert({FORTK, "FORTK"});
    str2type.insert({"scanf", SCANFTK});
    type2str.insert({SCANFTK, "SCANFTK"});
    str2type.insert({"printf", PRINTFTK});
    type2str.insert({PRINTFTK, "PRINTFTK"});
    str2type.insert({"return", RETURNTK});
    type2str.insert({RETURNTK, "RETURNTK"});

    type2str.insert({IDENFR, "IDENFR"});
    type2str.insert({INTCON, "INTCON"});
    type2str.insert({CHARCON, "CHARCON"});
    type2str.insert({STRCON, "STRCON"});
    type2str.insert({PLUS, "PLUS"});
    type2str.insert({MINU, "MINU"});
    type2str.insert({MULT, "MULT"});
    type2str.insert({DIV, "DIV"});
    type2str.insert({LSS, "LSS"});
    type2str.insert({LEQ, "LEQ"});
    type2str.insert({GRE, "GRE"});
    type2str.insert({GEQ, "GEQ"});
    type2str.insert({EQL, "EQL"});
    type2str.insert({NEQ, "NEQ"});
    type2str.insert({COLON, "COLON"});
    type2str.insert({ASSIGN, "ASSIGN"});
    type2str.insert({SEMICN, "SEMICN"});
    type2str.insert({COMMA, "COMMA"});
    type2str.insert({LPARENT, "LPARENT"});
    type2str.insert({RPARENT, "RPARENT"});
    type2str.insert({LBRACK, "LBRACK"});
    type2str.insert({RBRACK, "RBRACK"});
    type2str.insert({LBRACE, "LBRACE"});
    type2str.insert({RBRACE, "RBRACE"});
}
void LexAnalyzer::OUT()
{
    if (now_type == ILLEGAL || now_type == EOFTK || now_type == SIGQUO || now_type == DOUQUO)
        return;
#ifdef DEBUG
    cout << now_type << ' ' << str_read << endl;
#endif
    file_out << now_type << ' ' << str_read << endl;
}
void LexAnalyzer::OUT(int index)
{
    symType temsym = lexRes[index].first;
    if (temsym == ILLEGAL || temsym == EOFTK || temsym == SIGQUO || temsym == DOUQUO)
        return;
#ifdef DEBUG
    cout << temsym << ' ' << lexRes[index].second << endl;
#endif
    file_out << temsym << ' ' << lexRes[index].second << endl;
}
void LexAnalyzer::startAna()
{
    //initStr2Type();
    //now_ch = file_in.get(); // 必须
    //前面加了，上面俩行就注释了
    get_sym();
    while (now_type != EOFTK)
    {
#ifdef Lexout
        OUT();
#else
        lexRes.push_back({now_type, str_read});
#endif
        if (now_type == ILLEGAL)
        {
            lexRes.pop_back(); //保证最后的结果不出现非法字符例如单双引号
            if (str_read[0] == '\'')
            {
                //lexRes[lexRes.size() - 1].first = SIGQUO;
                get_nex_char();
#ifdef Lexout
                OUT();
#else
        lexRes.push_back({now_type, str_read});
#endif
            }
            else if (str_read[0] == '\"')
            {
                //lexRes[lexRes.size() - 1].first = DOUQUO;
                get_nex_string();
#ifdef Lexout
                OUT();
#else
        lexRes.push_back({now_type, str_read});
#endif
            }
        }
        get_sym();
    }
}
