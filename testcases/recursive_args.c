#include <stdio.h>

long long recursive_args(
        long long a,
        long long b,
        long long c,
        long long d,
        long long e,
        long long f,
        long long g,
        long long h,
        long long i) {
    if (a == 1) {
        return 1;
    } else {
        printf("a: %llu\tb: %llu\tc: %llu\n", a, b, c);
        printf("d: %llu\te: %llu\tf: %llu\n", d, e, f);
        printf("g: %llu\th: %llu\ti: %llu\n", g, h, i);
        long long ret = a + b - c + d - e + f - g + h - i;
        printf("ret: %llu  \n", ret);
        long long rec = recursive_args(a - 1, b, c, d, e, f, g, h, i);
        printf("rec: %llu\n\n", rec);
        return ret * rec;
    }
}

int main()
{
    long long ret = recursive_args(2, 8, 7, 6, 5, 4, 3, 2, 1);
    return ret;
}
