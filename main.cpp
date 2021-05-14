#include <iostream>
#include <vector>
#include <windows.h>

#include "functions.cpp"

int main(int argc, char** argv) {
    std::vector<std::string> arrStrings;
    if(argc > 1){
        readFileLines(arrStrings, argv[1]);
    } else {
        readFileLines(arrStrings, "data.txt");
    }

    for(int i = 0; i < arrStrings.size(); ++i) {
        ValuePos TempPair = calculate(arrStrings[i], 0, '\n');
        std::cout << "\nSource string N " << i+1 << ": " << arrStrings[i] << std::endl; 
        std::cout << "Result of work for string N " << i+1 << " is: " << TempPair.value << std::endl;
    }
    system("pause");
    return 0;
}
