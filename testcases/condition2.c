// test all conditions (that I know of at least...)
int main() {
    long long a = 3;
    long long c = 0;

    if (a < 10) {
        c += 1;
    }

    if (a > 2) {
        c += 1;
    }

    if (a >= 3) {
        c += 1;
    }

    if (a <= 3) {
        c += 1;
    }

    if (a <= 4) {
        c += 1;
    }

    if (a >= 2) {
        c += 1;
    }

    if (a == 3) {
        c += 1;
    }

    if (a != 2) {
        c += 1;
    }

    // 1 test to see if I am able to use !
    if (!(a == 2)) {
        c += 1;
    }

    // returns 9
    return c;
}
