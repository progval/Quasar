/*
 * stage2.c
 * Copyright (C) 2011 Leo Testard <leo.testard@gmail.com>
 *                    Florent Revest <florent.revest666@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Loading part of stage2
 * The stage2's job is to get multiboot info, load and jump to the 
 * kernel module.
 */

#include "stage2.h"

/* Macro that tells if the bit 'n' of 'flags' is set to 1 */
#define IS_SET(flags,n) ((flags >> n) & 0x1)
/* macro to reboot the computer */
#define reboot() outb(0x64, 0xFE)

void paging_init()
{
    unsigned int *page_directory = (unsigned int *)0x20000;
    unsigned int *page_table = (unsigned int *)0x21000;
    unsigned int page_addr = 0;
    int i;

    /* The first entry point to the first page table located just after the page directory */
    page_directory[0] = 0x21000 | 3;
    for(i = 1; i < 1024; i++)
        page_directory[i] = 0;

    for(i = 0; i < 1024; i++, page_addr += 4096)
        page_table[i] = page_addr | 3;

    asm("   mov %0, %%eax    \n \
            mov %%eax, %%cr3 \n \
            mov %%cr0, %%eax \n \
            or %1, %%eax     \n \
            mov %%eax, %%cr0" :: "i"(0x20000), "i"(0x80000000));
}

struct gdt_descriptors_table gdt_table;
struct gdt_segment_descriptor segments[256];

inline static void gdt_init_descriptor(int n, 
                                       unsigned int base, 
                                       unsigned int limit, 
                                       unsigned char access, 
                                       unsigned char flags)
{    
    /* put the values in the structure */
    segments[n].limit_low = limit & 0xFFFF;
    segments[n].limit_high = (limit & 0xF0000) >> 16;
    
    segments[n].base_low = base & 0xFFFF;
    segments[n].base_mid = (base >> 16) & 0xFF;
    segments[n].base_high = (base >> 24) & 0xFF;
    
    segments[n].access = access;
    segments[n].flags = flags & 0xF;
}

void gdt_init()
{
    /* We use 4 segments, the null segment, and one for code, data, and stack */ 
    gdt_init_descriptor(0, 0x0, 0x0, 0x0, 0x0);
	gdt_init_descriptor(1, 0x0, 0xFFFFF, 0x9B, 0x0D);
	gdt_init_descriptor(2, 0x0, 0xFFFFF, 0x93, 0x0D);
	gdt_init_descriptor(3, 0x0, 0x0, 0x97, 0x0D);	
	
	/* The total size of our descriptors is 256 descriptors x 8 bytes (each 
	   descriptor is 8-bytes long) */
	gdt_table.limit = 256*8;
	gdt_table.base = (unsigned int) &segments;
	
	/* tell the processor that we have and a brand new GDT */
	asm("lgdtl (gdt_table)");
	
	/* Load our segments */
	asm("   movw $0x10, %ax	\n \
            movw %ax, %ds	\n \
            movw %ax, %es	\n \
            movw %ax, %fs	\n \
            movw %ax, %gs	\n \
            ljmp $0x08, $next	\n \
            next:		\n");
            
}

/*
 * Entry point, called from the ASM file. Main method of stage2
 */
int main(unsigned long magic, struct multiboot *mboot)
{        
    char *video = (char *)0xB8000;
    char *cmdline;
    unsigned int bootdev, flags;
    int i = 0;
    gdt_init();
    paging_init();
    
    /* Are you Grub ? */
    if(magic == MULTIBOOT_BOOTLOADER_MAGIC)
    {
        printf("> Booting from a multiboot-compliant bootloader... \n");
        flags = mboot->flags;
        
        /* Read mem info : bit 0 indicates if mem fields are valids */
        if(IS_SET(flags, 0))
        {
            printf("> Lower memory : found %d Kb \n", mboot->mem_lower);
            printf("> Upper memory : found %d Kb \n", mboot->mem_upper);
        } else printf("> Unable to get memory info \n");
        
        /* Where are we booting from ? 
         * Bit 1 indicates if this field is ok */
        if(IS_SET(flags, 1))
        {
            bootdev = mboot->boot_device;
            printf("> Booting from BIOS device %d\n", (bootdev & 0xFF));
        } 
        
        /* Read the kernel command line */
        if(IS_SET(flags, 2))
        {
            cmdline = (char *) mboot->cmd_line;
            printf("> Kernel command line : %s \n", cmdline);
        }
    }
    else
        printf("> Booting from an unknown bootloader \n");
    
    idt_init();
    asm("sti");
    while(1);
}
