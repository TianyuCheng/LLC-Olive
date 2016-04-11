int main()
{
    long long data [] = { 4, 2, 6, 7, 8, 9, 10, 9, 2, 3, 4, 11, 10 };
    long long *ptr = data;
    long long max = *ptr;
    for (long long i = 0; i < 13; i++)
        if (max < *(ptr + i))
            max = *(ptr + i);
    return max;
}
