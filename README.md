# Vilkas x86 C++

A 64-bit x86 kernel I'm writing from scratch in C++ by reading the OSDEV wiki, Intel and AMD manuals.

It boots via GRUB, switches the CPU into long mode, and handles CPU exceptions with
a register dump.

## What works

- Multiboot boot via GRUB, 32-bit stub switches to long mode (PAE, EFER.LME, CR0.PG)
- 3-level page tables identity-mapping the first 1 GiB with 2 MiB huge pages
- 64-bit GDT and far jump into 64-bit code
- IDT with handlers for all 32 CPU exceptions
- Page faults report vector, error code, CR2 and RIP instead of triple faulting
- VGA text output, minimal freestanding libc (`libk`)

## Next

- Remap the PIC and handle hardware IRQs (timer, keyboard)
- Physical frame allocator off the multiboot memory map
- Proper 4 KiB paging with map/unmap, higher-half kernel

## Building

You need an `x86_64-elf` cross-compiler.

Build binutils and GCC for `--target=x86_64-elf` following the
[OSDev cross-compiler guide](https://wiki.osdev.org/GCC_Cross-Compiler), with one
extra step: patch GCC to build `libgcc` without the red zone
([instructions](https://wiki.osdev.org/Libgcc_without_red_zone)). Without it,
`libgcc` uses the 128-byte red zone that interrupts will clobber, and you get
corruption that's very hard to trace.

Then:

```sh
sudo apt install build-essential nasm xorriso grub-pc-bin qemu-system-x86
export PATH="$HOME/opt/cross/bin:$PATH"

./iso.sh
```

## Running

```sh
qemu-system-x86_64 -cdrom myos.iso
```

For debugging - this logs every interrupt the CPU takes and dumps state on a triple
fault, which is the only visibility you get when something goes wrong early:

```sh
qemu-system-x86_64 -cdrom myos.iso -d int,cpu_reset -no-reboot
```

## Layout

```
kernel/arch/x86_64/    boot stub, ISR stubs, GDT, VGA driver, linker script
kernel/kernel/         kernel_main, IDT setup, exception handler
libc/                  freestanding libc, built as libk for the kernel
```

## Notes
- [`docs/interrupts.md`](docs/interrupts.md) — IDT, stubs, and the interrupt frame

## Credits

Started from the OSDev wiki's [Meaty Skeleton](https://wiki.osdev.org/Meaty_Skeleton)
(public domain), ported to C++ and then to x86_64. The build system layout is still
theirs.
