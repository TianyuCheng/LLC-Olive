#include <stdio.h>

int main()
{
    long long ret = 0;
    for (long long i = 0; i < 10; i++) {
        printf("i: %lld\tret:%lld\n", i, ret);
        if (i == 5)
            break;
        ret += i;
    }
    return 0;
}
