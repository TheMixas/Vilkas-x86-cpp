#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/idt.h>
extern "C" void __cxa_pure_virtual()
{
	for (;;)
		__asm__("hlt");
}
extern "C" void kernel_main(void)
{
	terminal_initialize();
	idt_set_entries();
	idt_initialize();
	printf("Hello, kernel World in C++!\n");
	// printf("Hello, kernel World in C++!\n");
	// printf("Hello, kernel World in %d!\n", -2026);
	// asm volatile("int $3");
	// terminal_writestring("returned\n");
	//Test invalid opcode 
	// *(volatile int *)0 = 1;

	// Test page fault
	volatile uint64_t *p = (volatile uint64_t *)0x400000000ULL; // 16 GiB
	*p = 1;
	terminal_writestring("returned 2\n");
}
