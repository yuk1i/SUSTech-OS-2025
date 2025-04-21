CC := gcc
CFLAGS := -Wall -std=c11 -O2 -lpthread

PROGS := alipay alipay2 sum2 sum3 wait ab
PROGS_OPT := sum1_O0 sum1_O1 sum1_O2

all: $(PROGS) $(PROGS_OPT)

sum1_O0: sum1.c
	$(CC) $(CFLAGS) -O0 -o $@ $<

sum1_O1: sum1.c
	$(CC) $(CFLAGS) -O1 -o $@ $<

sum1_O2: sum1.c
	$(CC) $(CFLAGS) -O2 -o $@ $<

${PROGS}: %: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(PROGS) $(PROGS_OPT)