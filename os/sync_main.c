#include "defs.h"

void consumer(uint64);
void producer(uint64);
void synclab_main(uint64);

void synclab_init(void) {
    infof("sync lab init");
    create_kthread(synclab_main, 0);
}

#define BUFSIZE 5
#define NTHRS 10
#define NLOOPS 1000

struct buffer {
    // spinlock:
    spinlock_t lock;

    // data buffer:
    int guard0;
    int data[BUFSIZE];
    int guard1;

    int rpos;  // read position
    int wpos;  // write position
} buf;

static int is_full(struct buffer *buf) {
    return ((buf->wpos + 1) % BUFSIZE) == buf->rpos;
}

static int is_empty(struct buffer *buf) {
    return buf->wpos == buf->rpos;
}

uint64 counter[NTHRS];

void synclab_main(uint64) {
    memset(&counter, 0, sizeof(counter));
    memset(&buf, 0, sizeof(buf));
    buf.guard0 = 0xdeadbeef;
    buf.guard1 = 0xdeadbeef;
    spinlock_init(&buf.lock, "buffer");

    for (int i = 0; i < NTHRS; i++) {
        create_kthread(consumer, i);
    }
    for (int i = 0; i < NTHRS; i++) {
        create_kthread(producer, i);
    }
    while (wait(-1, NULL) > 0);

    infof("all threads finished");
    assert(buf.guard0 == 0xdeadbeef);
    assert(buf.guard1 == 0xdeadbeef);
    assert(is_empty(&buf));
    uint64 expected = NLOOPS * (NLOOPS - 1) / 2;
    for (int i = 0; i < NTHRS; i++) {
        assert(counter[i] == expected);
    }
    infof("all checks passed");
    exit(0);
}

void consumer(uint64 id) {
    infof("consumer %d starts", id);
    for (int i = 0; i < NLOOPS; i++) {
        int data;

        acquire(&buf.lock);
        while (is_empty(&buf)) sleep(&buf, &buf.lock);
        assert(!is_empty(&buf));
        
        data = buf.data[buf.rpos];
        buf.rpos = (buf.rpos + 1) % BUFSIZE;

        wakeup(&buf);
        release(&buf.lock);

        infof("consume %d: %x", id, data);

        int from_id = (data >> 16);
        uint64 value = (data & 0xffff);
        __sync_fetch_and_add(&counter[from_id], value);

        yield();
    }
    exit(0);
}

void producer(uint64 id) {
    infof("producer %d starts", id);
    for (int i = 0; i < NLOOPS; i++) {
        int data = (id << 16) | i;

        acquire(&buf.lock);
        while (is_full(&buf)) sleep(&buf, &buf.lock);
        assert(!is_full(&buf));

        buf.data[buf.wpos] = data;
        buf.wpos = (buf.wpos + 1) % BUFSIZE;

        wakeup(&buf);
        release(&buf.lock);

        infof("produce %d: %x", id, data);
        
        yield();
    }
    exit(0);
}