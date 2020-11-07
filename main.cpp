#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "mips.h"
#include "GrammaAna.h"
using namespace std;
ofstream mipsfile;
int main()
{
    ifstream fin("testfile.txt");
    ofstream fout("output.txt");
    ofstream err_fout("error.txt");
    mipsfile.open("mips.txt");
    errorInfo errHandler(err_fout);
    LexAnalyzer lexanaly(fin,fout,errHandler);
    //lexanaly.startAna();
    GrammaAna grammaAnaly(fin, fout, lexanaly);
    grammaAnaly.startGramAna();
    start_inter2mips();
    fin.close();
    err_fout.close();
    fout.close();
    mipsfile.close();
    return 0;
}
