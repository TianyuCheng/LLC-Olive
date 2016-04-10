#include <stdio.h>

int main()
{
    char fmt[6]; 
    fmt[0] = '%'; 
    fmt[1] = 'l'; 
    fmt[2] = 'l'; 
    fmt[3] = 'u'; 
    fmt[4] = '\n'; 
    fmt[5] = '\0';
    long long ret = 1;
    /* int ret = 1; */
    printf(fmt, ret);
    return 0;
}
