#ifndef SBI_H
#define SBI_H

#include "types.h"

struct sbiret {
	long error;
	long value;
};

static int inline sbi_call_legacy(uint64 which, uint64 arg0, uint64 arg1, uint64 arg2)
{
	register uint64 a0 asm("a0") = arg0;
	register uint64 a1 asm("a1") = arg1;
	register uint64 a2 asm("a2") = arg2;
	register uint64 a7 asm("a7") = which;
	asm volatile("ecall" : "=r"(a0) : "r"(a0), "r"(a1), "r"(a2), "r"(a7) : "memory");
	return a0;
}

static struct sbiret inline sbi_call(int32 eid, int32 fid, uint64 arg0, uint64 arg1, uint64 arg2)
{
	register uint64 a0 asm("a0") = arg0;
	register uint64 a1 asm("a1") = arg1;
	register uint64 a2 asm("a2") = arg2;
	register uint64 a6 asm("a6") = fid;
	register uint64 a7 asm("a7") = eid;
	asm volatile("ecall" : "=r"(a0), "=r"(a1) : "r"(a0), "r"(a1), "r"(a2), "r"(a6), "r"(a7) : "memory");
	struct sbiret ret;
	ret.error = a0;
	ret.value = a1;
	return ret;
}

void console_putchar(int);
int console_getchar();
void shutdown();
void set_timer(uint64 stime);

#endif  // SBI_H
