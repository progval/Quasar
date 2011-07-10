/* 
 * stage2.h
 * main header of stage2
 * contains defines and structs needed by the other components 
 * of Stage2 relatives to IDE management and Multiboot compliance.  
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

#endif /* STAGE2_H */