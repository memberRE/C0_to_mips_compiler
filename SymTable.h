#include <string>
#include <vector>
using namespace std;

enum IDENFR_TYPE
{
    ARRAY,
    VAR,
    CONSTT,
    FUNC,
    FUNC_NO_RET
};

enum TYPE_NAME
{
    INT,
    CHAR,
    STRING,
    VOID
};

class Symble_item
{
public:
    string name;
    IDENFR_TYPE iden_type;
    TYPE_NAME data_type;
    int dimen;
    int dimen_size[2];
    int const_val;
    bool has_ret;
    int addr;
    vector<TYPE_NAME> para_list; // 函数形参表
    Symble_item(string name,IDENFR_TYPE iden_type,TYPE_NAME data_type): name(name),iden_type(iden_type),data_type(date_type)
    {
        if (iden_type == FUNC)
            has_ret = 1;
        else
            has_ret = 0;
    }
    bool operator < (const Symble_item& other) const
    {
        return name < other.name;
    }
    bool operator > (const Symble_item& other) const
    {
        return name > other.name;
    }
    bool operator == (const Symble_item& other) const
    {
        return name == other.name;
    }
    bool operator != (const Symble_item& other) const
    {
        return name != other.name;
    }
};