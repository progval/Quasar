/*
 * disk.c
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
 * stage2.h
 * main header of stage2
 * contains defines and structs needed by the other components 
 * of Stage2 relatives to IDE and IDT management and Multiboot compliance.  
 */

#ifndef STAGE2_H
#define STAGE2_H

/* Macro that tells if the bit 'n' of 'flags' is set to 1 */
#define IS_SET(flags,n) ((flags >> n) & 0x1)
/* macro to reboot the computer */
#define reboot() outb(0x64, 0xFE)

/* Basic I/O ASM stubs */

inline void outb(unsigned short port, unsigned char value);
inline unsigned char inb(unsigned short port);
inline unsigned short inw(unsigned short port);

/* multiboot compliance definitions */

#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

struct multiboot {
    /* Multiboot version */
    unsigned int flags;

    /* Available memory */
    unsigned int mem_lower;
    unsigned int mem_upper;

    /* Main partition */
    unsigned int boot_device;

    /* Command line */
    unsigned int cmd_line;

    /* Boot-Module list */
    unsigned int mods_count;
    unsigned int mods_addr;
    
    unsigned int syms[4];
    
    /* mmap buffer */
    unsigned int mmap_length;
    unsigned int mmap_addr;
    
    /* Drive info */
    unsigned int drives_length;
    unsigned int drives_addr;
    
    /* ROM config table */
    unsigned int config_table;
    
    /* MBR name */
    unsigned int boot_loader_name;
    
    /* APM table */
    unsigned int apm_table;
    
    /* VBE */
    unsigned int vbe_control_info;
    unsigned int vbe_mode_info;
    unsigned short vbe_mode;
    unsigned short vbe_interface_seg;
    unsigned short vbe_interface_off;
    unsigned short vbe_interface_leng;
}__attribute__((packed));

/* IDT management structures */

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

extern void _asm_callback();
extern void _asm_callback_kbd();
extern void _asm_callback_pagefault();

/* 
 * VBE structures
 */

struct vesa_mode_info {
    unsigned short mod_attr;
    unsigned char win_attr[2];
    unsigned short win_grain;
    unsigned short win_size;
    unsigned short win_seg[2];
    unsigned short win_scheme;
    unsigned short logical_scan;

    unsigned short h_res;
    unsigned short v_res;
    unsigned char char_width;
    unsigned char char_height;
    unsigned char memory_planes;
    unsigned char bpp;
    unsigned char banks;
    unsigned char memory_layout;
    unsigned char bank_size;
    unsigned char image_planes;
    unsigned char page_function;

    unsigned char rmask;
    unsigned char rpos;
    unsigned char gmask;
    unsigned char gpos;
    unsigned char bmask;
    unsigned char bpos;
    unsigned char resv_mask;
    unsigned char resv_pos;
    unsigned char dcm_info;

    unsigned int lfb_ptr;
    unsigned int offscreen_ptr;
    unsigned short offscreen_size;

    unsigned char reserved[206];
};

#endif /* STAGE2_H */
