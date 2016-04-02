long long foo(long long a, 
              long long b, 
              long long c, 
              long long d, 
              long long e,
              long long f,      // up to here should be passed in registers
              long long g,
              long long h,
              long long i)      // these should be stored on stack
{
    return a + b + c + d + e + f + g + h + i;
}

int main()
{
    return foo(1, 2, 3, 4, 5, 6, 7, 8, 9);
}
