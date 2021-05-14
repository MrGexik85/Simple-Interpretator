#ifndef FUNCTIONS
#define FUNCTIONS

#include <string>
#include <vector>
#include <math.h>
#include <iostream>
#include <fstream>

#include "Token.cpp"
#include "ValuePos.cpp"

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
double merge(std::vector<Token> arrToken, Token &cur, int &index, bool mergeOneOnly = false){
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

// Функция для подсчета окончательного значения для переданной строки
ValuePos calculate(std::string curString, int curCursorPosition, char endSymbol){
    std::vector<Token> arrTokens;
    std::string buffer;

    // Переменная для хранения результата условия для токена 'if'
    bool condition;
    // Переменная для хранения количества повторений цикла 'for'
    int countRepeat = 0;

    // Было ли введено ключевое слово для сопоставления введенного численного значения с этим словом
    bool functionPresent = false;

    // Цикл, пока позиция курсора меньше длины строки и текущий символ не равен 'конечному' символу
    while(curCursorPosition < curString.length() && curString[curCursorPosition] != endSymbol){
        char curSymbol = curString[curCursorPosition];
        curCursorPosition++;
        
        // Если попалась буква, идет запись в буфер полного ключевого слова
        if(isLetter(curSymbol)){
            functionPresent = true;
            buffer += curSymbol;
            continue;
        }
        // Если попалась открывающаяся скобка, рекурсивно считаем значение внутри этой скобки, 'конечный' символ будет - закрывающая скобка
        if(curSymbol == '('){
            ValuePos tempPair = calculate(curString, curCursorPosition, ')');
            
            // Если ключевое слово было введено и оно соответсвует циклу 'for', то значение из скобок будет обозначать количество итераций
            if(functionPresent && buffer.compare("for") == 0){
                countRepeat = tempPair.value;
                buffer.clear();
                functionPresent = false;
            }
            // Если ключевое слово было введено и оно соответсвует 'do' то значение в скобках будет просуммировано countRepeat раз и создан новый токен 
            // с этим результатом и операцей '+'
            else if(functionPresent && buffer.compare("do") == 0){
                double result = countRepeat * tempPair.value;
                Token newToken = {result, '+'};
                arrTokens.push_back(newToken);
                buffer.clear();
                functionPresent = false;
            }
            // Если ключевое слово было введено и оно соответствует 'if', то значение в скобках будет рассмотрено как логическое выражение и если оно == 0,
            // то условие будет помечено как ложное, иначе истинное
            else if(functionPresent && buffer.compare("if") == 0){
                condition = (fabs(tempPair.value) < 1e-6);
                buffer.clear();
                functionPresent = false;
            }
            // Если ключевое слово было введено и оно соответсвует 'then' то значение в скобках будет выполнено при истинном значении condition и создан новый токен 
            // с этим результатом и операцей '+', иначе результат будет приравнен к нулю
            else if(functionPresent && buffer.compare("then") == 0){
                if(condition){
                    char buff[255] = {};
                    sprintf(buff, "%lf", tempPair.value);
                    buffer = buff;
                } else {
                    buffer = "0";
                    curSymbol = '+';
                }
                functionPresent = false;
                Token newToken = {atof(buffer.c_str()), curSymbol};
                arrTokens.push_back(newToken);
                buffer.clear();
            }
            // Если ключевое слово было введено и оно соответсвует 'else' то значение в скобках будет выполнено при ложном значении condition и создан новый токен 
            // с этим результатом и операцей '+', иначе результат будет приравнен к нулю
            else if(functionPresent && buffer.compare("else") == 0){
                if(!condition){
                    char buff[255] = {};
                    sprintf(buff, "%lf", tempPair.value);
                    buffer = buff;
                } else {
                    buffer = "0";
                }
                functionPresent = false;
                Token newToken = {atof(buffer.c_str()), '+'};
                arrTokens.push_back(newToken);
                buffer.clear();
            }
            // Если встречено какое-либо другое ключевое слово, оно отправляется в функцию для сопоставления его с заданными
            // функциями и для подсчета значения этой функции
            else if(functionPresent){
                char buff[255] = {};
                sprintf(buff, "%lf", calculateFunction(buffer, tempPair.value));
                buffer = buff;
                functionPresent = false; 
            } else {
                char buff[255] = {};
                sprintf(buff, "%lf", tempPair.value);
                buffer = buff;
            }
            // Обновление текущей позиции курсора (после уже прочтенного выражения в скобках)
            curCursorPosition = tempPair.position;
            curCursorPosition++;

            continue;
        }
        
        // Если символ может быть частью числа, то добавляем его в буффер
        if(isNumber(buffer, curSymbol, endSymbol))
        {
            if(curSymbol != ' '){
                buffer += curSymbol;
            }
            continue;
        }
        // Если позиция курсора в конце строки или символ равен 'конечному' символу, то поле action у нового токена будет ')' 
        if(curCursorPosition >= curString.length() || curString[curCursorPosition] == endSymbol)
        {
            curSymbol = ')';
        }

        Token newToken = {atof(buffer.c_str()), curSymbol};
        arrTokens.push_back(newToken);
        buffer.clear();
    }

    // Если в буффере еще что-то осталось, добавляем новый токен с action = ')'
    if(buffer.length() > 0)
    {
        Token newToken = {atof(buffer.c_str()), ')'};
        arrTokens.push_back(newToken);
        buffer.clear();
    }

    // Формируется окончательный ответ к рассмотренной строке
    Token firstToken = arrTokens[0];
    int index = 0;
    ValuePos ToReturn = {merge(arrTokens, firstToken, index), curCursorPosition};
    return ToReturn;
}

#endif