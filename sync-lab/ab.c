#include "thread.h"

largevar_t x, y;

flag_t wait;
#define FLAG1 0b01
#define FLAG2 0b10

__noinline__ void write_x_read_y() {
    int local;
    asm volatile(
        "movl $1, %0;"  // mov 1 (immediate) to x (%0)
        "movl %2, %1;"  // mov y (%2) to local (%1)
    : "=m"(x), "=r"(local)
    : "m"(y));
    printf("%d ", local);
}

__noinline__ void write_y_read_x() {
    int local;
    asm volatile(
        "movl $1, %0;"  // mov 1 (immediate) to y (%0)  , store y
        "movl %2, %1;"  // mov x (%2) to local (%1)     , load x
    : "=m"(y), "=r"(local)
    : "m"(x));
    printf("%d ", local);
}

void T1()
{
    while(1) {
        WAIT_FOR(flag_is_set(wait, FLAG1))
        write_x_read_y();
        flag_xor(wait, FLAG1);
    }
}

void T2()
{
    while(1) {
        WAIT_FOR(flag_is_set(wait, FLAG2))
        write_y_read_x();
        flag_xor(wait, FLAG2);
    }
}

int main()
{
    create(T1);
    create(T2);
    while (1)
    {
        // T1 and T2 are done
        x = 0;
        y = 0;
        barrier();
        usleep(1);
        assert(flag_get(wait) == 0);
        
        flag_xor(wait, FLAG1 | FLAG2);  // raise FLAG1 and FLAG2

        WAIT_FOR(flag_get(wait) == 0);

        printf("\n");
        fflush(stdout);
    }
}