#include<fstream>
#include <iostream>
#include<string>
using namespace std;
extern ofstream mipsfile;

void saveFp()
{
    mipsfile << "move $fp, $sp" << endl;
}

void changeSp(int len)
{
    mipsfile << "sub $sp, $sp, " << len << endl;
}

void setSp()
{
    mipsfile << "move $sp, $fp" << endl;
}

void addLable(string name)
{
    mipsfile << name << ":" << endl;
}

void jal(string name)
{
    mipsfile << "jal " << name << endl;
}

void syscall(int type)
{
    mipsfile << "li $v0, " << type << endl;
    mipsfile << "syscall" << endl;
}

void sw(string reg, string offset)
{
    mipsfile << "sw " << reg << ", " << offset << endl;
}

void sw(string reg, string offset,string loca)
{
    mipsfile << "sw " << reg << ", " << offset << "(" << loca << ")" << endl;
}

void li(string reg, int data)
{
    mipsfile << "li " << reg << ", " << data << endl;
}

void la(string reg, string tar)
{
    mipsfile << "la " << reg << ", " << tar << endl;
}

void lw(string reg, string offset)
{
    mipsfile << "lw " << reg << ", " << offset << endl;
}

void lw(string reg, string offset,string loca)
{
    mipsfile << "lw " << reg << ", " << offset << "(" << loca << ")" << endl;
}

void jr(string reg)
{
    mipsfile << "jr " << reg << endl;
}

void j(string tar)
{
    mipsfile << "j " << tar << endl;
}

void beq(string tar, string reg1, string reg2)
{
    mipsfile << "beq " << reg1 << ", " << reg2 << ", " << tar << endl;
}

void bne(string tar, string reg1, string reg2)
{
    mipsfile << "bne " << reg1 << ", " << reg2 << ", " << tar << endl;
}

void bgt(string tar, string reg1, string reg2)
{
    mipsfile << "bgt " << reg1 << ", " << reg2 << ", " << tar << endl;
}

void bge(string tar, string reg1, string reg2)
{
    mipsfile << "bge " << reg1 << ", " << reg2 << ", " << tar << endl;
}

void blt(string tar, string reg1, string reg2)
{
    mipsfile << "blt " << reg1 << ", " << reg2 << ", " << tar << endl;
}

void ble(string tar, string reg1, string reg2)
{
    mipsfile << "ble " << reg1 << ", " << reg2 << ", " << tar << endl;
}

void move(string dst, string src)
{
    mipsfile << "move " << dst << ", " << src << endl;
}

void add(string dst, string op1, string op2)
{
    mipsfile << "add " << dst << ", " << op1 << ", " << op2 << endl;
}

void sub(string dst, string op1, string op2)
{
    mipsfile << "sub " << dst << ", " << op1 << ", " << op2 << endl;
}

void mult(string dst, string op1, string op2)
{
    mipsfile << "mult " << op1 << ", " << op2 << endl;
    mipsfile << "mflo " << dst << endl;
}

void div(string dst, string op1, string op2)
{
    mipsfile << "div " << op1 << ", " << op2 << endl;
    mipsfile << "mflo " << dst << endl;
}