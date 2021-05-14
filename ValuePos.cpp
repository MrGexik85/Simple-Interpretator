#ifndef VALUEPOS_STRUCT
#define VALUEPOS_STRUCT

/* 
/ Вспомогательная стуктура 'значение-позиция'
/ Хранит в себе значение из только что посчитанной скобки и позицию, куда нужно поставить курсор
*/
struct ValuePos {
    double value;
    int position;
};

#endif