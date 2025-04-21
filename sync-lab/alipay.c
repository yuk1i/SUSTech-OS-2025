#include "thread.h"

unsigned long money = 30;

void deduct() {
    if (money >= 1) {
        usleep(1);
        money -= 1;
    }
}

int main() {
    for (int i = 0; i < 100; i++) create(deduct);
    join();
    printf("money = %lu\n", money);
}