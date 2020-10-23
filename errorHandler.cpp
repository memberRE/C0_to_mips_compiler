#include "errorHandler.h"
#include "LexAla.h"
#include <algorithm>
#include <map>
#include <vector>
#define DEBUG
using namespace std;

map<int, char> err2Char;

ostream &operator<<(ostream &ofs, ERROR_TYPE st)
{
    ofs << err2Char[st];
    return ofs;
}

void errorInfo::initErr2Char()
{
    err2Char.insert({ILLEGAL_CHAR, 'a'});
    err2Char.insert({NAME_REDEF, 'b'});
    err2Char.insert({NAME_NODEF, 'c'});
    err2Char.insert({PARA_NUM_NOT_MATCH, 'd'});
    err2Char.insert({PARA_TYPE_NOT_MATCH, 'e'});
    err2Char.insert({CONDITION_TYPE_ERR, 'f'});
    err2Char.insert({NORET_FUNC_ERR, 'g'});
    err2Char.insert({RET_FUNC_ERR, 'h'});
    err2Char.insert({INDEX_ERR, 'i'});
    err2Char.insert({CONST_WRI_ERR, 'j'});
    err2Char.insert({NO_SEM, 'k'});
    err2Char.insert({NO_RPARENT, 'l'});
    err2Char.insert({NO_RBRACK, 'm'});
    err2Char.insert({ARRAY_INIT_ERR, 'n'});
    err2Char.insert({TYPE_NOT_MATCH, 'o'});
    err2Char.insert({NO_DEFAULT, 'p'});
}

void errorInfo::OUTALL()
{
    sort(errorSet.begin(), errorSet.end());
    for (pair<int, ERROR_TYPE> T : errorSet)
    {
#ifdef DEBUG
        cout << T.first << ' ' << T.second << endl;
#endif
#ifndef ERROR_HANDLAR
        file_out << T.first << ' ' << T.second << endl;
#endif
    }
}

void errorInfo::add(int line_num, ERROR_TYPE e)
{
    errorSet.push_back({line_num, e});
}
