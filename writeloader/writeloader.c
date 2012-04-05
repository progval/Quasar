#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
 * bootloader installer :
 * install Quasar first bootloader (given as first parameter)
 * and stage1 (given as second parameter)
 * on a hard drive image with 1 DOS partition (given as third parameter)
 */

 void usage(const char *argv0)
 {
    printf("Quasar bootloader installer : \n");
    printf("Usage: %s <bootloader> <chain> <stage1> <disk image> \n", argv0);
    printf("Help: install the flat binary <bootloader> on the 446 first \n"
        "octets of <disk image>, and the binary <stage1> on the reserverd \n"
        "sectors of the first partition, that should be FAT32.\n"
        "<chain> will be installed on the boot section of the partition\n"
        "VBR to chainload from <bootloader> to <stage1>\n");
 }

int bad_usage(const char *argv0)
{
    usage(argv0);
    return EXIT_FAILURE;
}

int error(const char *msg)
{
    return EXIT_FAILURE;
}

int open_disk_image(const char *path, FILE *f)
{
    if(f == NULL)
    {
        printf("Error: Disk image %s couldn't be opened", path);
        perror("Error");
        return 1;
    }

    unsigned char buffer[512];
    if(fread((void *) buffer, 1, 512, f) != 512)
    {
        printf("Error: failed to read %s\n", path);
        perror("Error");
        return 1;
    }

    if(buffer[510] != 0x55 || buffer[511] != 0xAA)
    {
        printf("Error: %s doesn't seem to have a valid partition table\n",path);
        return 1;
    }

    return 0;
}

int open_binary(const char *path, FILE *f, int max_size)
{   
    if(f == NULL)
    {
        printf("Error: Binary %s couldn't be opened\n", path);
        return 1;
    } 

    fseek(f, 0L, SEEK_END);
    if(ftell(f) > max_size)
    {
        printf("Error: binary %s is too great ! It should be smaller than "
            "%d bytes \n", path, max_size);
        return 1;
    }

    fseek(f, 0L, SEEK_SET);
    return 0;
}

int write_stages(const char *stage1_path, const char *stage2_path, FILE *disk)
{
    /* localize the FS on the disk */
    unsigned char part_table[64];
    int i;

    fseek(disk, 0x1BE, SEEK_SET);

    if(fread((void *) part_table, 1, 64, disk) != 64)
    {
        printf("Error: Failed to read disk partition table\n");
        perror("Error");
        return 1;
    }

    for(i = 0; i < 4; i++)
    {
        int offset = 16*i;
        unsigned char buf[420] = { 0 };
        int byte_addr;
        FILE *stage1;
        FILE *stage2;

        /* check if partition is bootable */
        if(part_table[offset] != 0x80)
            continue;

        /* check FS (must be FAT32) */
        if(part_table[offset+4] != 0x0C)
            continue;

	   printf("Writing Stage1 on partition %d of disk...\n", i);

        /* write on this partition, get its LBA address */
        unsigned int addr = (part_table[offset+8])
            + (part_table[offset+9] << 8)
            + (part_table[offset+10] << 16)
            + (part_table[offset+11] << 24);

        /* boot code is at beginning of partition + 0x3E */
        byte_addr = addr*512 + 0x3E;

        /* now, read the stage1 binary */
        stage1 = fopen(stage1_path, "r");
        if(open_binary(stage1_path, stage1, 420))
            return 1;

        if(fread((void *)buf, 1, 420, stage1) != 420)
        {
            if(ferror(stage1))
            {
                printf("Error: Impossible to read %s\n", stage1_path);
                perror("Error");
                return 1;
            }

            /* we reached the end of the file */
        }

        fseek(disk, byte_addr, SEEK_SET);
        if(fwrite(buf, 1, 420, disk) != 420)
        {
            printf("Error: Impossible to write chainloader\n");
            perror("Error");
            return 1;
        }

        printf("Writing stage2...\n");

        /* write stage1 at the first reserved sector, IE the 2cnd sector */
        byte_addr = addr*512 + 0x200;

        stage2 = fopen(stage2_path, "r");
        if(open_binary(stage2_path, stage2, 32*512))
            return 1;

        unsigned char stage2_buf[32*512];

        if(fread((void *)stage2_buf, 1, 32*512, stage2) != 32*512)
        {
            if(ferror(stage2))
            {
                printf("Error: Impossible to read %s\n", stage2_path);
                perror("Error");
                return 1;
            }
        }

        fseek(disk, byte_addr, SEEK_SET);
        if(fwrite(stage2_buf, 1, 32*512, disk) != 32*512)
        {
            printf("Error: Impossible to write chainloader\n");
            perror("Error");
            return 1;
        }

        return 0;
    }

    /* if we arrive here, it means no suitable FS has been found */
    printf("Error: Disk does not contains any DOS bootable partition\n");
    return 1;
}

int write_bootloader(const char *bootloader_path, FILE *disk)
{
    unsigned char buf[446] = { 0 };
    FILE *loader;

    printf("Writing main bootloader on Master Boot record...\n");

    loader = fopen(bootloader_path, "r");
    if(open_binary(bootloader_path, loader, 446))
        return 1;

    if(fread((void *)buf, 1, 446, loader) != 446)
    {
        if(ferror(loader))
        {
            printf("Error: Impossible to read %s\n", bootloader_path);
            perror("Error");
            return 1;
        }

        /* we reached the end of the file */
    }

    fseek(disk, 0L, SEEK_SET);
    if(fwrite(buf, 1, 446, disk) != 446)
    {
        printf("Error: Impossible to write bootloader\n");
        perror("Error");
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    const char *loader_path;
    const char *stage1_path;
    const char *stage2_path;
    const char *disk_path;
    FILE *disk;

    printf("Writeloader Quasar bootloader flasher V 0.1\n");
    printf("Copyright (C) 2012 - Leo Testard\n"); 

    if(argc == 1)
    {
        if(strcmp(argv[1], "--help"))
            usage(argv[0]);
        return EXIT_SUCCESS;
    }

    if(argc < 4)
        return bad_usage(argv[0]);

    loader_path = argv[1];
    stage1_path = argv[2];
    stage2_path = argv[3];
    disk_path = argv[4];

    disk = fopen(disk_path, "r+");


    /* read the partition table and ensure it is correct */

    if(open_disk_image(disk_path, disk))
        return EXIT_FAILURE;

    /* write stage1 on file system */
    if(write_stages(stage1_path, stage2_path, disk))
        return EXIT_FAILURE;

    if(write_bootloader(loader_path, disk))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
