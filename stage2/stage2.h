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
 * of Stage2 relatives to IDE and GDT management and Multiboot compliance.  
 */

#ifndef STAGE2_H
#define STAGE2_H

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

/* IDE management definitions */

#define ATA_DISK_MASTER         0x1
#define ATA_DISK_SLAVE          0x2
#define ATA_DISK                0x3
#define ATAPI_DISK              0x4

#define ATA_DATA                0x00
#define ATA_ERROR               0x01
#define ATA_PRECOMP             0x01
#define ATA_SECTOR_COUNT        0x02
#define ATA_SECTOR_NUMBER       0x03
#define ATA_CYL_LSB             0x04
#define ATA_CYL_MSB             0x05

#define ATA_DRIVE               0x06
#define ATA_DRIVE_IBM           0xA0
#define ATA_DRIVE_LBA           0x40
#define ATA_DRIVE_MASTER        0x00
#define ATA_DRIVE_SLAVE         0x10

#define ATA_STATUS              0x07
#define ATA_STATUS_ERROR        0x01
#define ATA_STATUS_INDEX        0x02
#define ATA_STATUS_CORR         0x04
#define ATA_STATUS_DRQ          0x08
#define ATA_STATUS_DSC          0x10
#define ATA_STATUS_DWF          0x20
#define ATA_STATUS_DRDY         0x40
#define ATA_STATUS_BUSY         0x80

#define ATA_CMD                 0x07
#define ATA_CMD_ATA_IDENT       0xEC
#define ATA_CMD_ATAPI_IDENT     0xA1
#define ATA_CMD_READ            0x20
#define ATA_CMD_WRITE           0x30
#define ATA_CMD_READ_MULTI      0xC4
#define ATA_CMD_WRITE_MULTI     0xC5
#define ATA_CMD_SET_MULTI       0xC6
#define ATA_CMD_PACKET          0xA0

#define ATA_ALTPORT             0x206
#define ATA_DEVICE_CONTROL      0x206
#define ATA_N_IEN               0x02
#define ATA_RESET               0x04
#define ATA_4BIT                0x08

/* File systems magic numbers */

#define NO_FS                   0x0
#define FS_EXT                  0x83

/* definitions related to GDT management */

/*
 * Describe the in-RAM layout of a GDT segment descriptor. This structure 
 * describes a memory segment
 */
struct gdt_segment_descriptor {
    unsigned short limit_low;
	unsigned short base_low;
	unsigned char base_mid;
   	unsigned char access;
    unsigned char limit_high: 4;
	unsigned char flags:4;
	unsigned char base_high;
}__attribute__((packed));

/*
 * This one describes the in-RAM layout of the GDT table itself, that contains
 * the address and the size of the segment descriptors
 */
struct gdt_descriptors_table {
    unsigned short limit;
    unsigned int base;
}__attribute__((packed));


#endif /* STAGE2_H */
