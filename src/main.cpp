// **************************************************************************
//  File       [main.cpp]
//  Author     [Samuel Liu]
//  Synopsis   [the main program of the CycleBreaking]
//  Modify     [2022/01/11 Samuel Liu]
// **************************************************************************

#include <iostream>
#include "CycleBreaking.h"
using namespace std;

void help_message()
{
    cout << "usage: ./bin/cb <input file name> <output file name> \n" << endl;
}

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
       help_message();
       return 0;
    }

    CycleBreaking CycleBreaking;
    CycleBreaking.ReadFile(argv[1]);
    CycleBreaking.Process();
    CycleBreaking.WriteFile(argv[2]);
}