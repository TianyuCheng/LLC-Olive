#include <stdio.h>

int main()
{
    long long i;
    long long ret = 0;
    for (i = 0; i < 10; i++) {
        ret += i;
    }
    return ret;
}
