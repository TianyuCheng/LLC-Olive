#include <stdio.h>

int main()
{
    char fmt[4];
    fmt[0] = '%'; 
    fmt[1] = 'd'; 
    fmt[2] = '\n'; 
    fmt[3] = '\0';
    printf(fmt, 1);
    return 0;
}
