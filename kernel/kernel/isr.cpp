#include <kernel/tty.h>
#include <kernel/idt.h>
#include <stdio.h>
static const char *names[32] = {
    "Divide by zero",
    "Debug Exception",
    "Non-maskable interrupt",
    "Breakpoint",
    "Overflow",
    "Bound range exceeded",
    "Invalid opcode",
    "Device not available",
    "Double fault",
    "Coprocessor segment overrun",
    "Invalid TSS",
    "Segment not present",
    "Stack-segment fault",
    "General protection fault",
    "Page fault",
    "Reserved",
    "x87 floating-point exception",
    "Alignment check",
    "Machine check",
    "SIMD floating-point exception",
    "Virtualization exception",
    "Control protection exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
};
extern "C" void isr_handler(InterruptFrame *frame)
{
    uint64_t cr2;
    asm volatile("mov %%cr2, %0" : "=r"(cr2));
    // if(frame->vector == 14){
    //     printf("Page fault at address: %x\n", cr2); todo: add %x to printf to print hex values
    // }
    if (frame->vector < 32)
    {
        printf("Interrupt happened: %s\n", names[frame->vector]);
    }

    if (frame->vector == 3)
    {
        return;
    }
    else
    {
        for (;;)
        {
            asm volatile("hlt");
        }
    }
}