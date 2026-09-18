# Declare constants for the multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard.
.section .multiboot
# Multiboot header is required to be 4-byte aligned.
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .rodata
gdt64:
	.quad 0x0                    # null
	.quad (1<<43) | (1<<44) | (1<<47) | (1<<53)   # code
	.quad (1<<41) | (1<<44) | (1<<47)             # data
gdt64_pointer:
	.word gdt64_pointer - gdt64 - 1
	.long gdt64

.section .bss
.align 4096
pml4_table:
.skip 4096 # 4 KiB
.align 4096
pdpt_table:
.skip 4096 # 4 KiB
.align 4096
pd_table:
.skip 4096 # 4 KiB

# Reserve a stack for the initial thread.
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:





# The kernel entry point.
.section .text
.global _start
.type _start, @function
.code32
_start:
	movl $stack_top, %esp
	# --- Build the page tables ---------------------------------------

	# pml4_table[0] = pdpt_table | PRESENT | WRITABLE
	movl $pdpt_table, %eax
	orl  $0x03, %eax
	movl %eax, pml4_table
	movl $0,   pml4_table+4

	# pdpt_table[0] = pd_table | PRESENT | WRITABLE
	movl $pd_table, %eax
	orl  $0x03, %eax
	movl %eax, pdpt_table
	movl $0,   pdpt_table+4

	# pd_table[i] = (i * 2MiB) | PRESENT | WRITABLE | HUGE, i = 0..511
	# Identity-maps the first 1 GiB.
	movl $0, %ecx
1:
	movl %ecx, %eax
	shll $21, %eax                  # i * 2 MiB
	orl  $0x83, %eax                # present | writable | page-size
	movl %eax, pd_table(,%ecx,8)
	movl $0,   pd_table+4(,%ecx,8)
	incl %ecx
	cmpl $512, %ecx
	jb 1b

	# --- CR3 = physical base of PML4 ---------------------------------
	movl $pml4_table, %eax
	movl %eax, %cr3


	# Enable physical-address extensions by setting CR4.PAE to 1
	mov %cr4, %eax
	or $(1<<5), %eax
	mov %eax, %cr4

	# Enable long mode by setting EFER.LME to 1
	mov $0xC0000080, %ecx
	rdmsr
	or $(1 << 8), %eax
	wrmsr
	
	# Load the GDT and switch to 64-bit mode.
	lgdt gdt64_pointer

	# TODO: CR0.PG = 1, then lgdt + far jump into a 64-bit segment.
	mov %cr0, %eax
	or $(1<<31), %eax
	mov %eax, %cr0

	ljmp $0x08, $_long_mode_start
.size _start, . - _start




.code64
_long_mode_start:
	# Clear GRUBS GDTS segment registers.
	movw $0x10, %ax
	movw %ax, %ss
	movw %ax, %ds
	movw %ax, %es
	
	# Call the global constructors.
	call _init

	# Transfer control to the main kernel.
	call kernel_main

	# Hang if kernel_main unexpectedly returns.
	cli
1:	hlt
	jmp 1b
	
