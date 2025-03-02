#include "sbi.h"
#include "types.h"
#include "riscv.h"

// Legacy:
const uint64 SBI_SET_TIMER = 0;
const uint64 SBI_CONSOLE_PUTCHAR = 1;
const uint64 SBI_CONSOLE_GETCHAR = 2;
const uint64 SBI_CLEAR_IPI = 3;
const uint64 SBI_SEND_IPI = 4;
const uint64 SBI_REMOTE_FENCE_I = 5;
const uint64 SBI_REMOTE_SFENCE_VMA = 6;
const uint64 SBI_REMOTE_SFENCE_VMA_ASID = 7;
const uint64 SBI_SHUTDOWN = 8;

// SBI Extension: Specify EID and FID.
const uint64 SBI_EID_BASE = 0x10;

void console_putchar(int c) {
    sbi_call_legacy(SBI_CONSOLE_PUTCHAR, c, 0, 0);
}

int console_getchar() {
    return sbi_call_legacy(SBI_CONSOLE_GETCHAR, 0, 0, 0);
}

void shutdown(void) {
	while (1);	
}