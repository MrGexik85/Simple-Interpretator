#ifndef FUNCTIONS
#define FUNCTIONS

#include <string>
#include <vector>
#include <math.h>
#include <iostream>

#include "Token.cpp"

// Функция возвращает приоритет операций
int getActionPriority(char action) {
    switch(action) {
        case '*':
        case '/': return 3;
        case '+':
        case '-': return 2;
        case '>':
        case '<':
        case '=': return 1;
    }
    return -1;
}

// Функция узнает можно ли совершить операцию, исходя из приоритета операций у двух токенов
bool canMerge(Token tokenLeft, Token tokenRight) {
    return getActionPriority(tokenLeft.action) >= getActionPriority(tokenRight.action);
}

// Слияние двух токенов
void mergeTokens(Token &tokenLeft, Token &tokenRight){
    switch(tokenLeft.action){
        case '*':
            tokenLeft.value = tokenLeft.value * tokenRight.value;
            break;
        case '/':
            tokenLeft.value = tokenLeft.value / tokenRight.value;
            break;
        case '+':
            tokenLeft.value = tokenLeft.value + tokenRight.value;
            break;
        case '-':
            tokenLeft.value = tokenLeft.value - tokenRight.value;
            break;
        case '>':
            tokenLeft.value = tokenLeft.value < tokenRight.value;
            break;
        case '<':
            tokenLeft.value = tokenLeft.value > tokenRight.value;
            break;
        case '=':
            tokenLeft.value = tokenLeft.value == tokenRight.value;
            break;
    }
    tokenLeft.action = tokenRight.action; 
}

// Возвращает: является ли переданный символ символом операции
bool isAction(char symbol){
    return symbol == '+' || symbol == '-' || symbol == '*' || symbol == '/' || symbol == '>' || symbol == '<' || symbol == '=';
}

// Возвращает: является ли переданный символ символом буквой
bool isLetter(char symbol){
    return (symbol >= 'A' && symbol <= 'Z') || (symbol >= 'a' && symbol <= 'z');
}

// Принимает строку-идентификатор и сопоставляет к ней функцию и высчитывает значение
// Для ключевых слов "do", 'then', 'else', 'for' функция не выполняет арифметическую операцию
double calculateFunction(std::string nameOfFunction, double value){
    if(nameOfFunction.compare("exp") == 0){
        return exp(value);
    } else if(nameOfFunction.compare("sin") == 0){
        return sin(value);
    } else if(nameOfFunction.compare("cos") == 0){
        return cos(value);
    }
    return value;
}

// Выполнение слияния
double merge(std::vector<Token> arrToken, Token &cur, int &index, bool mergeOneOnly){
    while(index < arrToken.size() && cur.action != ')'){
        index++;
        Token temp = arrToken[index];
        while(!canMerge(cur, temp)){
            merge(arrToken, temp, index, true);
        }
        mergeTokens(cur, temp);
        if(mergeOneOnly){
            return cur.value;
        }
    }
    return cur.value;
}

// Может ли быть переданный символ частью числа
bool isNumber(std::string str, char symbol, char endSymbol){
    char end = (endSymbol == ')' || endSymbol == '\n') ? ')' : endSymbol;

    return (!(symbol == '(' || symbol == end || isAction(symbol)) ||
          (str.length() < 1 && (symbol == '+' || symbol == '-' || symbol == ')')));
}

#endif