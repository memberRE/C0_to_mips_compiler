#include<fstream>
#include<iostream>
#include<string>
using namespace std;
enum symType{
    IDENFR,//��ʶ��
    INTCON,//���� ��
    CHARCON,//�ַ�����
    STRCON,//�ַ���
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
    LexAnalyzer (ifstream &ff_in,ofstream & ff_out) : file_in(ff_in),file_out(ff_out) {
        initStr2Type();
        now_ch = file_in.get();
    }
private:

};
