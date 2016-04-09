int main() {
    long long a = 3;
    long long b = 4;

    long long c = 0;

    if (a < 10) {
        // go here
        c = 4;
    } else {
        // do not go here
        c = 2;
    }

    if (b > 5) {
        // do not go here
        c++;
    } else if (b > 3) {
        // go here
        c += 4;
        // c now 8
    } else {
        // do not go here
        c = 7;
    }

    if (a < 10 && b > 3) {
        // go here
        c++;
        // c now 9
    }

    if (a > 10) {
        // do not go here
        c = -90;
    } else {
        // go here
        c = c + a;
        // c now 12
    }

    
    // returns 12
    return c;
}
