#include <stdio.h>

int main()
{
    long long data [] = { 4, 2, 6, 7, 8, 9, 10, 9, 2, 3, 4, 11, 10 };
    long long *ptr = data;
    long long max = data[0];
    for (long long i = 0; i < 13; i++) {
        long long m = *(ptr + i);
        printf("m: %lld\n", m);
        if (max < m)
            max = m;
    }
    return max;
}
