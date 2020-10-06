#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "GrammaAna.h"
using namespace std;
int main()
{
    ifstream fin("testfile.txt");
    ofstream fout("output.txt");
    LexAnalyzer lexanaly(fin,fout);
    //lexanaly.startAna();
    GrammaAna grammaAnaly(fin, fout, lexanaly);
    grammaAnaly.startGramAna();
    fin.close();

    fout.close();
}
