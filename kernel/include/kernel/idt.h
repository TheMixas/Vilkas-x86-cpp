#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
struct [[gnu::packed]] GeneralPurposeRegisters {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
};
static_assert(sizeof(GeneralPurposeRegisters) == 15 * 8);

struct [[gnu::packed]] InterruptFrame {
    GeneralPurposeRegisters regs;
    uint64_t vector;
    uint64_t error_code;
    uint64_t rip, cs, rflags, rsp, ss;
};
static_assert(sizeof(InterruptFrame) == 22 * 8);

struct [[gnu::packed]] InterruptDescriptor64 {
    uint16_t offset_1;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  type_attributes;
    uint16_t offset_2;
    uint32_t offset_3;
    uint32_t zero;
};

struct [[gnu::packed]] InterruptDescriptorTablePointer {
    uint16_t size;
    uint64_t base;
};
static_assert(sizeof(InterruptDescriptor64) == 16);

void idt_initialize(void);
void idt_set_entries(void);


#ifdef __cplusplus
}
#endif
