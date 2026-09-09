#include <stdio.h>

#include <kernel/tty.h>
extern "C" void __cxa_pure_virtual() { for (;;) __asm__("hlt"); }
extern "C" void kernel_main(void) {
	terminal_initialize();
	printf("Hello, kernel World in C++!\n");
}
