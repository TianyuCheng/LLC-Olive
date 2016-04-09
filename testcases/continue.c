int main(int argc, char *argv[])
{
    long long ret = 0;
    for (long long i = 0; i < 10; i++) {
        if (i == 5)
            continue;
        ret += i;
    }
    return ret;
}
