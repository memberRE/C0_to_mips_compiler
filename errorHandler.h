#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

enum ERROR_TYPE
{
    ILLEGAL_CHAR,
    NAME_REDEF,
    NAME_NODEF,
    PARA_NUM_NOT_MATCH,
    PARA_TYPE_NOT_MATCH,
    CONDITION_TYPE_ERR,
    NORET_FUNC_ERR,
    RET_FUNC_ERR,
    INDEX_ERR,
    CONST_WRI_ERR,
    NO_SEM,
    NO_RPARENT,
    NO_RBRACK,
    ARRAY_INIT_ERR,
    TYPE_NOT_MATCH,
    NO_DEFAULT
};

class errorInfo
{
public:
    vector<pair<int,ERROR_TYPE>> errorSet;
    ofstream &file_out;
    void initErr2Char();
    void OUTALL();
    void add(int line_num, ERROR_TYPE e);
    errorInfo(ofstream &ff_out) : file_out(ff_out)
    {
        initErr2Char();
    }
private:
};