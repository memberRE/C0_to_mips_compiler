#include<fstream>
#include<iostream>
#include<string>
#include "errorHandler.h"
using namespace std;
enum symType{
    IDENFR,
    INTCON,
    CHARCON,
    STRCON,
    CONSTTK,//const
    INTTK,//int
    CHARTK,//char
    VOIDTK,//
    MAINTK,
    IFTK,
    ELSETK,
    SWITCHTK,
    CASETK,
    DEFAULTTK,
    WHILETK,
    FORTK,
    SCANFTK,
    PRINTFTK,
    RETURNTK,
    PLUS,
    MINU,
    MULT,
    DIV,
    LSS,
    LEQ,
    GRE,
    GEQ,
    EQL,
    NEQ,
    COLON,
    ASSIGN,
    SEMICN,
    COMMA,
    LPARENT,
    RPARENT,
    LBRACK,
    RBRACK,
    LBRACE,
    RBRACE,
    EOFTK,
    ILLEGAL,
    SIGQUO,
    DOUQUO
};

struct SYM_INFO{
    symType first;
    string second;
    int line_num;
};

class LexAnalyzer
{
public:
    char now_ch;
    void get_sym();
    void get_nex_char();
    void get_nex_string();
    void startAna();
    void OUT();
    void initStr2Type();
    void OUT(int index);
    ifstream &file_in;
    ofstream &file_out;
    errorInfo &errHandle;
    LexAnalyzer (ifstream &ff_in,ofstream & ff_out,errorInfo & e) : file_in(ff_in),file_out(ff_out),errHandle(e) {
        initStr2Type();
        now_ch = file_in.get();
    }
private:

};
