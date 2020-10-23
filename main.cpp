#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "errorHandler.h"
#include "GrammaAna.h"
using namespace std;
int main()
{
    ifstream fin("testfile.txt");
    ofstream fout("output.txt");
    ofstream err_fout("error.txt");
    errorInfo errHandler(err_fout);
    LexAnalyzer lexanaly(fin,fout,errHandler);
    //lexanaly.startAna();
    GrammaAna grammaAnaly(fin, fout, lexanaly);
    grammaAnaly.startGramAna();
    fin.close();
    err_fout.close();
    fout.close();
}
