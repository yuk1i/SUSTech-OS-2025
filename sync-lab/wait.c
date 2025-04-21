#include "thread.h"

int flag = 0;

void T1() {
    usleep(100);
    flag = 1;
}

void T2() {
    while(flag == 0);
    printf("T2 got it.\n");
}

int main() {
    create(T1);
    create(T2);
    join();
}