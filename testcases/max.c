int main()
{
    long long arr[] = { 5, 1, 4, 2, 6, 7, 8, 9 };
    long long ret = arr[0];
    for (long long i = 0; i < 8; i++)
        if (ret < arr[i])
            ret = arr[i];
    return ret;
}
