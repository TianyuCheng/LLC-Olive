int main()
{
    long long arr[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    long long ret = arr[0];
    for (long long i = 0; i < 8; i++)
        if (arr[i] > ret)
            ret = arr[i];
    return ret;
}
