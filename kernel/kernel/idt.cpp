#include <stdint.h>
#include <kernel/idt.h>

alignas(16) InterruptDescriptor64 idt[256] = {};
void idt_set_entry(uint8_t index, uint64_t handler, uint8_t flags) {
    idt[index].offset_1 = handler & 0xFFFF;
    idt[index].selector = 0x08;
    idt[index].ist = 0;
    idt[index].type_attributes = flags;
    idt[index].offset_2 = (handler >> 16) & 0xFFFF;
    idt[index].offset_3 = (handler >> 32) & 0xFFFFFFFF;
    idt[index].zero = 0;
}

InterruptDescriptorTablePointer idt_ptr = { sizeof(idt) - 1, (uint64_t)&idt};
extern "C" void isr3();
extern "C" void* isr_stub_table[];

//Load the IDT into the CPU's IDT register
void idt_set_entries() {
    for (int i = 0; i < 32; i++){
        idt_set_entry(i, (uint64_t)isr_stub_table[i], 0x8E);
    }

}
void idt_initialize() {
    __asm__ volatile("lidt %0" :: "m"(idt_ptr));

}