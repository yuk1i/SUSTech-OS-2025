#include "console.h"
#include "defs.h"

extern char e_text[];  // kernel.ld sets this to end of kernel code.
extern char s_bss[];
extern char e_bss[];
extern char ekernel[], skernel[];

uint64 global_variable = 0xdeadbeef;

void print_sysregs() {
    uint64 sstatus = r_sstatus();
    uint64 scause = r_scause();
    uint64 sie = r_sie();
    uint64 sepc = r_sepc();
    uint64 stval = r_stval();
    uint64 sip = r_sip();
    uint64 satp = r_satp();
    printf("sstatus : %p\n", sstatus);
    printf("scause  : %p\n", scause);
    printf("sepc    : %p\n", sepc);
    printf("stval   : %p\n", stval);
    printf("sip     : %p\n", sip);
    printf("sie     : %p\n", sie);
    printf("satp    : %p\n", satp);
}

void main() {
    printf("\n");
    printf("clean bss: %p - %p\n", s_bss, e_bss);
    memset(s_bss, 0, e_bss - s_bss);
    printf("Kernel booted.\n");
    printf("Hello World!\n");

    printf("sysregs:\n");
    print_sysregs();
    printf("%d\n", global_variable);

    printf("kernel ends, parking...\n");

    while (1);
}
