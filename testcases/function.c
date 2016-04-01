int foo(int a, 
        int b, 
        int c, 
        int d, 
        int e,
        int f,      // up to here should be passed in registers
        int g,
        int h,
        int i)      // these should be stored on stack
{
    return a + b + c + d + e + f + g + h + i;
}

int main()
{
    return foo(1, 2, 3, 4, 5, 6, 7, 8, 9);
}
