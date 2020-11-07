#include <string>
using namespace std;

void saveFp();
void changeSp(int len);
void setSp();
void addLable(string name);
void jal(string name);
void syscall(int type);
void sw(string reg, string offset);
void sw(string reg, string offset, string loca);
void lw(string reg, string offset, string loca);
void lw(string reg, string offset);
void li(string reg, int value);
void la(string reg, string tar);
void jr(string reg);
void j(string tar);
void beq(string tar, string reg1, string reg2);
void bne(string tar, string reg1, string reg2);
void bgt(string tar, string reg1, string reg2);
void bge(string tar, string reg1, string reg2);
void blt(string tar, string reg1, string reg2);
void ble(string tar, string reg1, string reg2);
void add(string dst, string op1, string op2);
void sub(string dst, string op1, string op2);
void mult(string dst, string op1, string op2);
void div(string dst, string op1, string op2);