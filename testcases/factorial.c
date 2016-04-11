#include <stdio.h>

long long factorial(long long a) {
    if (a == 1) {
        return 1;
    } else {
        return a * factorial(a - 1);
    }
}

int main() {
    // 5 * 4 * 3 * 2 * 1 = 120
    return factorial(5);
}
