#include "thread.h"
#define N 100000000
volatile long sum = 0;

void T_sum() {
    bindtocpu(0);
    for (int i = 0; i < N; i++) {
        // sum++;
        asm volatile("incq %0":"+m"(sum));
    }
}

int main() {
    create(T_sum);
    create(T_sum);
    join();
    printf("sum = %ld\n", sum);
}