#ifndef FAT_H
#define FAT_H

#include "disk.h"

/* structures for FAT file system management */

struct fat_bootsector {
    unsigned char boot_jump_code[3];
    unsigned char dos_version[8];
    unsigned short bytes_per_sector;
    unsigned char sectors_per_cluster;
    unsigned short reserved_sectors;
    unsigned char allocation_tables;
    unsigned short directory_entries;
    unsigned short sectors;
    unsigned char media_descriptor_type;
    unsigned short table_size_16;
    unsigned short sectors_per_track;
    unsigned short heads;
    unsigned int hidden_sectors;
    unsigned int large_volume_sectors;
};

struct fat32_extended_bootsector {
    unsigned int sectors_per_fat;
    unsigned short flags;
    unsigned char fat_version_major;
    unsigned char fat_version_minor;
    unsigned int rootdir_cluster;
    unsigned short fsinfo_cluster;
    unsigned short backup_cluster;
    unsigned char zeroes[12];
    unsigned char drive_number;
    unsigned char winnt_flags;
    unsigned char signature;
    unsigned int serial;
    unsigned char label[11];
    unsigned char sys_identifier[8];
};

#endif