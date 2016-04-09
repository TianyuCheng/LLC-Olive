int main() {
    long long a = 3;
    long long b = 4;
    long long c = 0;

    // note this only goes for 2 iterations
    for (long long i = 0; i < a; i++) {
        c += 2;

        if (i == 1) {
            break;
        }
    }
    // c = 4 now
    
    long long d = 0;

    // skip iterations 0, 1, 2
    // go when d == 3, d == 4
    while (d < 4) {
        d++;

        if (d <= 2) {
            continue;
        }
        c += 3;
    }
    
    // returns 7
    return c;
}
