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
#include "disk.h"

/*
 * Entry point, called from the ASM file. Main method of stage2
 */
int main(unsigned long magic, struct multiboot *mboot)
{        
    char *video = (char *)0xB8000;
    char *cmdline;
    unsigned int bootdev, flags;
    int i = 0;

    unsigned int *page_directory = (unsigned int *)0x20000;
    unsigned int *page_table = (unsigned int *)0x21000;
    unsigned int page_addr = 0;

    /* Are you Grub ? */
    if(magic == MULTIBOOT_BOOTLOADER_MAGIC) {
        printf("> Booting from a multiboot-compliant bootloader... \n");
        flags = mboot->flags;

        /* Read mem info : bit 0 indicates if mem fields are valids */
        if(IS_SET(flags, 0)) {
            printf("> Lower memory : found %d Kb \n", mboot->mem_lower);
            printf("> Upper memory : found %d Kb \n", mboot->mem_upper);
        }
        else
            printf("> Unable to get memory info \n");

        /* Where are we booting from ? 
         * Bit 1 indicates if this field is ok */
        if(IS_SET(flags, 1)) {
            bootdev = mboot->boot_device;
            printf("> Booting from BIOS device %d\n", (bootdev & 0xFF));
        }

        /* Read the kernel command line */
        if(IS_SET(flags, 2)) {
            cmdline = (char *) mboot->cmd_line;
            printf("> Kernel command line : %s \n", cmdline);
        }
        
        if(IS_SET(flags, 11)) { // Damn, it works!
            struct vesa_mode_info *vmi = (struct vesa_mode_info *) mboot->vbe_control_info;
            char *videoMemory = (char *) vmi->lfb_ptr;
            int width = (int) vmi->v_res;
            int height = (int) vmi->h_res;

            // Fill the screen with white
            int w, h;
            for (h = 0; h < height; h++)
            {
                for (w = 0; w < width; w++)
                {
                    *videoMemory++ = 255; // Blue
                    *videoMemory++ = 255; // Green
                    *videoMemory++ = 255; // Red
                }
            }
        }
    }
    else
        printf("> Booting from an unknown bootloader \n");

    /* 
     * The limit of our IDT register is the size in bytes of the table. We know 
     * our IDT contains 256 descriptors, that are 4-words long. The total size
     * is of 8 bytes * 256 = 2048 bytes.
     */
    idt_table.limit = 256*sizeof(struct idt_interrupt_descriptor) - 1;
    idt_table.base = (unsigned int) &descriptors;

    /* Setup the PIC, I.E. the Programmable Interrupt Controller */
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    /* Initialize all interrupts with a default callback */
    for(i = 0; i < 256; i++)
    {    
        descriptors[i].callback_low = ((unsigned int) _asm_callback) & 0xFFFF;
        descriptors[i].callback_high = (((unsigned int) _asm_callback) >> 16) & 0xFFFF;
        descriptors[i].selector = 0x08;
        descriptors[i].zero = 0;
        descriptors[i].flags = 0x8E;
    }

    /* override the keyboard interrupt, as it will need it's own callback */
    descriptors[33].callback_low = ((unsigned int) _asm_callback_kbd) & 0xFFFF;
    descriptors[33].callback_high = (((unsigned int) _asm_callback_kbd) >> 16) & 0xFFFF;
    descriptors[33].selector = 0x08;
    descriptors[33].zero = 0;
    descriptors[33].flags = 0x8E;

    /* add another callback for the page fautls */
    descriptors[14].callback_low = ((unsigned int) _asm_callback_pagefault) & 0xFFFF;
    descriptors[14].callback_high = (((unsigned int) _asm_callback_pagefault) >> 16) & 0xFFFF;
    descriptors[14].selector = 0x08;
    descriptors[14].zero = 0;
    descriptors[14].flags = 0x8E;

		
    /* Tell the processor where the new IDT is */
    asm("lidt (idt_table)");    
    asm("sti");

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

    struct drive *ret = find_drives();
    list_drives(ret);

    while(1);
}

void c_callback_pagefault()
{
    printf("PAGE FAULT !!!!!\n");
}

