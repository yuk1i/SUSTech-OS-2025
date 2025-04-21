#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200112L

#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdatomic.h>

#ifndef asm
#define asm __asm__
#endif

#define MAX_THREADS 100

static pthread_t tids[MAX_THREADS];
static int tid_count = 0;

/**
 * Create a thread
 */
void create(void (*fn)(void)) {
    pthread_t tid;
    pthread_create(&tid, NULL, (void*(*)(void*)) fn, NULL);
    // pthread_detach(tid);
    tids[tid_count++] = tid;
}

void join(void) {
    for(int i=0;i<tid_count;i++) {
        assert(pthread_join(tids[i], NULL) == 0);
    }
}

void bindtocpu(int cpuid) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpuid, &cpuset);
    assert(pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset) == 0);
}

typedef __attribute__((aligned(64))) int largevar_t;

// Safe Flag:

typedef volatile atomic_int flag_t;
#define barrier() __sync_synchronize()

#define flag_xor(flg, bit) (atomic_fetch_xor(&(flg), (bit)))
#define flag_get(flg) (atomic_load(&(flg)))
#define flag_is_set(flg, bitmask) ((flag_get((flg)) & (bitmask)))

#define WAIT_FOR(cond) {while(!(cond)) asm volatile("":::"memory");}

#define __noinline__ __attribute((noinline))