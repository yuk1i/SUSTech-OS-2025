#include "thread.h"

#define N 100000000
long sum = 0;

void T_sum() {
    for (int i = 0; i < N; i++) {
        sum++;
    }
}

int main() {
    create(T_sum);
    create(T_sum);
    join();
    printf("sum = %ld\n", sum);
}