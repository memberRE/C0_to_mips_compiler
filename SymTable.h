#pragma once
#include <set>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

extern int now_addr_offset;

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
    int tot_memory;
    int addr;
    string name_space;
    vector<TYPE_NAME> para_list; // 函数形参表
    Symble_item(string name, IDENFR_TYPE iden_type, TYPE_NAME data_type) : name(name), iden_type(iden_type), data_type(data_type)
    {
        if (iden_type == FUNC)
            has_ret = 1;
        else
            has_ret = 0;
        addr = tot_memory = 0;
        dimen = 0;
        dimen_size[0] = dimen_size[1] = 0;
        const_val = 0;
        name_space = "global";
    }

    void out(ofstream& file_ot) const
    {
        file_ot << name << "\t" << iden_type << "\t" << data_type << "\t" << addr << "\t" <<endl;
    }

    int gen_addr()
    {
        this->addr = now_addr_offset;
        now_addr_offset += 4;
        return this->addr;
    }

    bool operator<(const Symble_item &other) const
    {
        return name < other.name;
    }
    bool operator>(const Symble_item &other) const
    {
        return name > other.name;
    }
    bool operator==(const Symble_item &other) const
    {
        return name == other.name;
    }
    bool operator!=(const Symble_item &other) const
    {
        return name != other.name;
    }
};

struct RUN_TAB
{
    string name;
    set<Symble_item> tab;
    RUN_TAB(string name, set<Symble_item>& tab) : name(name), tab(tab) {

    }
};