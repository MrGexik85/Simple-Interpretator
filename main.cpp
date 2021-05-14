#include <iostream>
#include <vector>
#include <cstring>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <windows.h>
#include <math.h>

#include "Token.cpp"
#include "functions.cpp"

struct Pair {
    double Value;
    int Position;
};

// Чтение и добавление в массив (vector) интерпретируемых строк 
void readFileLines(std::vector<std::string> &arrLines, std::string filename) {
    std::ifstream in(filename.c_str());

    if(!in.is_open())
    {
        std::cout << "Can not open file" << std::endl;
        exit(-1);
    }

    std::string Temp;
    while(getline(in, Temp))
    {
        arrLines.push_back(Temp);
    }
}


Pair Calculate(std::string CurrentLine, int OpeningSymbolPosition, char FinalSymbol)
{
    std::vector<Token> VectorToFill;
    std::string Buffer;

    bool Condition;
    int Repetitions = 0;

    bool FunctionPresent = false;

    while(OpeningSymbolPosition < CurrentLine.length() && CurrentLine[OpeningSymbolPosition] != FinalSymbol)
    {
        char Symbol = CurrentLine[OpeningSymbolPosition];
        OpeningSymbolPosition++;

        if(isLetter(Symbol))
        {
            FunctionPresent = true;

            Buffer += Symbol;
            if(CurrentLine[OpeningSymbolPosition] != FinalSymbol && OpeningSymbolPosition < CurrentLine.length())
            {
                continue;
            }
        }

        if(Symbol == '(')
        {
            Pair TempPair = Calculate(CurrentLine, OpeningSymbolPosition, ')');

            if(FunctionPresent && Buffer == "for")
            {
                Repetitions = calculateFunction(Buffer, TempPair.Value);
                Buffer.clear();
                FunctionPresent = false;
            }
            else if(FunctionPresent && Buffer == "do")
            {
                double result;
                for(int i = 0; i < Repetitions; ++i)
                {
                    result += calculateFunction(Buffer, TempPair.Value);
                }
                Symbol = '+';
                Token NewToken = {result, Symbol};
                VectorToFill.push_back(NewToken);
                Buffer.clear();
                FunctionPresent = false;
            }
            else if(FunctionPresent && Buffer == "if")
            {
                if(fabs(TempPair.Value) < 0.000001)
                    Condition = false;
                else Condition = true;
                Buffer.clear();
                FunctionPresent = false;
            }
            else if(FunctionPresent && Buffer == "then")
            {
                if(Condition)
                {
                    char buffer[100] = {};
                    sprintf(buffer, "%lf", calculateFunction(Buffer, TempPair.Value));
                    Buffer = buffer;
                    FunctionPresent = false;
                }
                else
                {
                    Buffer = "0";
                    Symbol = '+';
                    FunctionPresent = false;
                }
                Token NewToken = {atof(Buffer.c_str()), Symbol};
                VectorToFill.push_back(NewToken);
                Buffer.clear();
            }
            else if(FunctionPresent && Buffer == "else")
            {
                if(!Condition)
                {
                    char buffer[100] = {};
                    sprintf(buffer, "%lf", calculateFunction(Buffer, TempPair.Value));
                    Buffer = buffer;
                    FunctionPresent = false;
                    Symbol = '+';
                }
                else
                {
                    Buffer = "0";
                    Symbol = '+';
                    FunctionPresent = false;
                }
                Token NewToken = {atof(Buffer.c_str()), Symbol};
                VectorToFill.push_back(NewToken);
                Buffer.clear();
            }
            else if(FunctionPresent)
            {
                char buffer[100] = {};
                sprintf(buffer, "%lf", calculateFunction(Buffer, TempPair.Value));
                Buffer = buffer;
                FunctionPresent = false; //
            }
            else
            {
                char buffer[100] = {};
                sprintf(buffer, "%lf", TempPair.Value);
                Buffer = buffer;
            }

            OpeningSymbolPosition = TempPair.Position;
            OpeningSymbolPosition++;

            continue;
        }

        if(isNumber(Buffer, Symbol, FinalSymbol))
        {
            if(Symbol != ' ')
                Buffer += Symbol;
            if(CurrentLine[OpeningSymbolPosition] != FinalSymbol && OpeningSymbolPosition < CurrentLine.length())
            {
                continue;
            }
        }

        if(OpeningSymbolPosition >= CurrentLine.length() || CurrentLine[OpeningSymbolPosition] == FinalSymbol)
        {
            Symbol = ')';
        }

        Token NewToken = {atof(Buffer.c_str()), Symbol};
        VectorToFill.push_back(NewToken);
        Buffer.clear();
    }

    if(Buffer.length() > 0)
    {
        Token NewToken = {atof(Buffer.c_str()), ')'};
        VectorToFill.push_back(NewToken);
        Buffer.clear();
    }

    Token FirstToken = VectorToFill[0];
    int Index = 0;

    Pair ToReturn = {   merge(VectorToFill, FirstToken, Index, false),    OpeningSymbolPosition   };
    return ToReturn;
}

// -------------------------------------- \\
//                  main                  \\
// -------------------------------------- \\

int main(int argc, char** argv) {
    std::vector<std::string> arrStrings;
    if(argc > 1){
        readFileLines(arrStrings, argv[1]);
    } else {
        readFileLines(arrStrings, "data.txt");
    }

    for(int i = 0; i < arrStrings.size(); ++i) {
        Pair TempPair = Calculate(arrStrings[i], 0, '\n');
        std::cout << "\nSource string N " << i+1 << ": " << arrStrings[i] << std::endl; 
        std::cout << "Result of work for string N " << i+1 << " is: " << TempPair.Value << std::endl;
    }
    system("pause");
    return 0;
}
