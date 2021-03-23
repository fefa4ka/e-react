#include "common.h"


//-----------------------------------------------------------------------------
// Преобразование числа в строку ANSI C через быстрое деление
//-----------------------------------------------------------------------------

static char utoaBuffer[13];

static unsigned int g_seed = 12312;


inline void divmodu10(divmod10_t *res, unsigned long n)
{
    res->quot = n >> 1;
    res->quot += res->quot >> 1;
    res->quot += res->quot >> 4;
    res->quot += res->quot >> 8;
    res->quot += res->quot >> 16;
    unsigned long qq = res->quot;

    res->quot >>= 3;
    res->rem = (unsigned short)(n - ((res->quot << 1) + (qq & ~7ul)));
    if(res->rem > 9)
    {
        res->rem -= 10;
        res->quot++;
    }
}

extern inline int random() { 
  g_seed = (214013*g_seed+2531011); 
  return (g_seed>>16)&0x7FFF; 
} 

//utoa fast div
char * utoa(unsigned long value)
{
    char* buffer = &utoaBuffer[1];
    buffer += 11;
    *--buffer = 0;
    do
    {
        divmod10_t res = {0};
        divmodu10(&res, value);
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


unsigned char reverse(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}
