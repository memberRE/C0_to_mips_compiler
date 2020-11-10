#include <string>
#include <iostream>
#include <fstream>
using namespace std;
class Inter
{
public:
    string op;
    string iden1;
    string iden2;
    string tar;
    Inter(string op, string iden1, string iden2, string tar)
        : op(op), iden1(iden1), iden2(iden2), tar(tar)
    {
    }
    void out(ofstream& file_ot)
    {
        file_ot << op << "," << iden1 << "," << iden2 << "," << tar << endl;
    }
};