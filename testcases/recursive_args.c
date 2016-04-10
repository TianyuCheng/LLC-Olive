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
        long long ret = a + b - c + d - e + f - g + h - i;
        return recursive_args(a - 1, b, c, d, e, f, g, h, i);
    }
}

int main()
{
    long long ret = recursive_args(9, 8, 7, 6, 5, 4, 3, 2, 1);
    return ret;
}
