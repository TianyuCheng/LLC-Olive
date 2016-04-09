// return 7 + 6; test function, no args
long long stuff() {
    long long b = 1;
    long long c = 2;
    long long d = 3;

    long long e = b + c; // 3
    long long f = e + d; // 6

    return 7 + f; // 7 + 6  = 13
}

int main() {
    long long k = stuff();
    
    // returns 13
    return k;
}
