// test bunch of args
long long stuff(long long a, long long b, long long c, long long d, long long e,
                long long f, long long g, long long h, long long j) {
    return a + b + c + d + e + f + g + h + j;
}

int main() {
    long long k = stuff(1,1,1,1,1,1,2,2,2);
    
    // returns 12
    return k;
}
