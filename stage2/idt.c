/*
 * idt.c
 * Copyright (C) 2011 Leo Testard <leo.testard@gmail.com>
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
 * Minimal IDT management
 * This IDT is used only to setup a basic keyboard management for our embedded
 * shell. 
 * All that the idt_init function does is to initialize all descriptors with
 * our default callback function and then override the keyboard specific int 
 * with the _asm_callback_bkd that get keyboard events
 * Callback functions are defined in interrupts_callbacks.s and calls C 
 * callbacks defined at the end of this file.
 */
 
/* Those functions are called when an interrupt is triggered. They are defined 
 * in interrupts_callbacks.s, as they need to be in ASM.  
 */
extern void _asm_callback();
extern void _asm_callback_kbd();

/*
 * Describe the in RAM layout of an interrupt descriptor. This descriptor
 * contains various infos, the most important is the address of the callback 
 * function that should be called when the interrupt is triggered.
 * As this struct match the in RAM layout, the names of members variables are
 * not explicit. See idt_init_descriptor for a definition.
 */
struct idt_interrupt_descriptor {
    unsigned short callback_low;
    unsigned short selector;
    unsigned char  zero;
    unsigned char  flags;
    unsigned short callback_high;
}__attribute__((packed));

/*
 * Describe the in RAM layout of the table itselfs, that contains the address
 * and the size of the descriptors 
 */
struct idt_descriptors_table {
    unsigned short limit;
    unsigned int base;
}__attribute__((packed));

struct idt_descriptors_table idt_table;
struct idt_interrupt_descriptor descriptors[256];

/*
 * This registers an interrupt in the IDT table :
 * int_num is the number of the interrupt (I.E. 33 is keyboard and 0 divide err)
 * callback is a linear address of a function that is called when this int is 
 * triggered
 * selector is the kernel segment selector
 * flags are various flags such as access, etc.
 */
inline static void idt_init_descriptor(unsigned char int_num, 
                                       unsigned int callback, 
                                       unsigned short selector, 
                                       unsigned char flags)
{    
    /* put the values in the structure */
    descriptors[int_num].callback_low = (callback & 0xFFFF);
    descriptors[int_num].callback_high = (callback >> 16) & 0xFFFF;
    descriptors[int_num].selector = selector;
    descriptors[int_num].zero = 0;
    descriptors[int_num].flags = flags;
}

void idt_init()
{
    int i;
    
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
        idt_init_descriptor(i, (unsigned int) _asm_callback, 0x08, 0x8E);
        
    /* override the keyboard interrupt, as it will need it's own callback */
    idt_init_descriptor(33, (unsigned int) _asm_callback_kbd, 0x08, 0x8E);
		
    /* Tell the processor where the new IDT is */
    asm("lidtl (idt_table)");
}

void c_callback()
{
    /* this callback does nothing for the moment */
    return;
}

/* This function is called when a keyboard event is triggered */
void c_callback_kbd()
{
    printf("The keyboard has been triggered \n");
}
