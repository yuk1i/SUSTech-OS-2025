#include "thread.h"

unsigned long money = 30;

void deduct() {
    long local_money;
    do {
        local_money = money;
        if (local_money == 0)
            break;
        usleep(1);
    } while(!__sync_bool_compare_and_swap(&money, local_money, local_money-1));
}

int main() {
    for (int i = 0; i < 100; i++) create(deduct);
    join();
    printf("money = %lu\n", money);
}