#include "common.h"

char * utoa(unsigned long value);
char * itoa(long value);

//-----------------------------------------------------------------------------
// Преобразование числа в строку ANSI C через быстрое деление
//-----------------------------------------------------------------------------

static char utoaBuffer[13];
typedef struct 
{
    unsigned long quot;
    unsigned short rem;
} divmod10_t;



inline static divmod10_t divmodu10(unsigned long n)
{
    divmod10_t res;

    res.quot = n >> 1;
    res.quot += res.quot >> 1;
    res.quot += res.quot >> 4;
    res.quot += res.quot >> 8;
    res.quot += res.quot >> 16;
    unsigned long qq = res.quot;

    res.quot >>= 3;
    res.rem = (unsigned short)(n - ((res.quot << 1) + (qq & ~7ul)));
    if(res.rem > 9)
    {
        res.rem -= 10;
        res.quot++;
    }
    return res;
}



//utoa fast div
char * utoa(unsigned long value)
{
    char* buffer = &utoaBuffer[1];
    buffer += 11;
    *--buffer = 0;
    do
    {
        divmod10_t res = divmodu10(value);
        *--buffer = res.rem + '0';
        value = res.quot;
    }
    while (value != 0);
    return buffer;
}




//utoa fast div
char * itoa(long value)
{
    //Чёто неохото было гуглить. Мой
    //тупой вариант взятия модуля
    unsigned long unsignedValue = value;
    if (value < 0)
        unsignedValue = (unsigned long)(value - (2*value));
    
    char* unsignedStr = utoa(unsignedValue);
    
    //Добавляем знак минус в начало
    if (value < 0) {
        unsignedStr--;
        *unsignedStr = '-';       
    }
           
    return unsignedStr;
}

