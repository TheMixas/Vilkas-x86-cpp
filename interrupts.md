# Interrupts

How exceptions are handled in long mode.

## How it works

On an exception the CPU reads `idt[vector]`, pushes a frame, and jumps to the
handler address stored in that entry. `IDTR` (set by `lidt`) tells it where the
table is.

Same pattern as `GDTR`/GDT and `CR3`/page tables: a register points at a table in
RAM, hardware walks it.

## Stubs

Some vectors push an error code, most don't: **8, 10, 11, 12, 13, 14, 17, 21, 29,
30** do. So two macros — the no-error variant pushes a dummy zero, and both push
the vector number, so every handler sees the same layout.

`isr_common` then:

1. pushes all 15 GPRs (without rsp)
2. `movq %rsp, %rdi` and calls `isr_handler` (Pass the InterruptFrame as a parameter in isr_handler)
3. pops in exact reverse order
4. `addq $16, %rsp` to drop the vector and error code
5. `iretq`

## The frame

`rsp` points at exactly this:

```cpp
struct __attribute__((packed)) InterruptFrame {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t vector, error_code;
    uint64_t rip, cs, rflags, rsp, ss;
};
```

Field order is the **reverse** of push order — the stack grows down.

## The handler

Page fault address is in `CR2`, nowhere else:

```cpp
asm volatile("mov %%cr2, %0" : "=r"(cr2));
```

Error code bits: 0 = present, 1 = write, 2 = user. So `0x2` = write to a
non-present page.

Don't return from a fault — it re-executes the faulting instruction and loops
forever. Traps (3, 4) are fine to return from.

## Testing

```cpp
asm volatile("int $3");                                  // should print, then resume

volatile uint64_t* p = (volatile uint64_t*)0x400000000ULL;  // 16 GiB, unmapped
*p = 1;                                                  // #PF, error 0x2
```

`*(int*)0 = 1` does **not** page fault — address 0 is inside our identity mapping.
GCC also rewrites it to `ud2`, so you get `#UD` instead.

## Todo

- Remap the PIC to vectors 32–47 before `sti` (defaults collide with 8–15)
- IST for a double-fault stack — needs a TSS

