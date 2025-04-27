#include "../lib/user.h"

void worker() {
    for(int i = 0; i < 10; i++) {
        printf("worker %d: %d\n", getpid(), i);
        
        for(long j = 0; j < 50000000; j++) {
            asm volatile("" : : : "memory");
            // do nothing
        }
        
        sleep(100);
    }
}

int main(void) {
    // fork 5 times
    for (int i = 0; i < 5; i++) {
        int pid = fork();
        if (pid < 0) {
            printf("init: fork failed\n");
            exit(1);
        }
        if (pid == 0) {
            // child process
            worker();
            printf("worker %d: exit\n", getpid());
            exit(0);
        }
    }
    // wait all children
    while (wait(-1, NULL) > 0);
    return 0;
}