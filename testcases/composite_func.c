long long foo(long long a) {
    return a - 1;
}

int main(int argc, char *argv[])
{
    return foo(foo(5));
}
