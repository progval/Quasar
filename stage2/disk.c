/*
 * disk.c
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
 * Minimal IDE drives management of stage2. 
 * The whereami() function lists IDE devices presents. Currently, it
 * can only read ATA devices. When such a device is found, it lists its
 * primary partitions and print their FS type according to the magic
 * number present in the partition table. This is done by the 
 * identify_fs() function.
 */ 

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
#define FS_FAT32                0x0C
#define FS_EXT                  0x83

#include "disk.h"
#include "fat.h"

/*
 * Loops for the given delay. Used to wait the end of an I/O operation.
 */
static inline void udelay(unsigned int delay)
{
    int i;
    for(i = 0; i < (delay*1000); i++)
    {
        i++;
        i--;
    }
}

/*
 * Returns the name of the FS according to the magic number.
 * number-to-fs correspondance is defined in stage2.h
 */
static char *identify_fs(unsigned char magic_number)
{
    switch(magic_number)
    {
        case NO_FS:
            return "Empty partition";
        case FS_EXT:
            return "Linux EXT";
        case FS_FAT32:
            return "FAT 32";
        default:
            return "Unknown FS";
    }
}

/*
 * Read a single sector (located at the offset 'sector') on the given
 * drive, and place it in 'buf'. 'port' is the processor port of the
 * device that should be used, and master indicates if the drive is the
 * master or slave drive on the controller
 */
static void 
ide_read(long sector, void *buf, unsigned short port, char master)
{
    unsigned char cyl_low, cyl_high, sect, head, status;
    int devselect, i, timeout;
    unsigned char *buffer;
    
    if(master == ATA_DISK_MASTER)
        devselect = ATA_DRIVE_MASTER;
    else
        devselect = ATA_DRIVE_SLAVE;

    outb(port + ATA_DEVICE_CONTROL, ATA_N_IEN | ATA_4BIT);
    udelay(1);

    sect = sector & 0xFF;
    cyl_low = (sector >> 8) & 0xFF;
    cyl_high = (sector >> 16) & 0xFF;
    head = ((sector >> 24) & 0x7) | 0x40;

    /* select the port and the drive */
    outb(port + ATA_DRIVE, ATA_DRIVE_IBM | devselect);
    udelay(100);

    /* select the sector */
    outb(port + ATA_DEVICE_CONTROL, ATA_4BIT);
    outb(port + ATA_ERROR, 1);
    outb(port + ATA_PRECOMP, 0);
    outb(port + ATA_SECTOR_COUNT, 1);
    outb(port + ATA_SECTOR_NUMBER, sect);
    outb(port + ATA_CYL_LSB, cyl_low);
    outb(port + ATA_CYL_MSB, cyl_high);
    outb(port + ATA_DRIVE, ATA_DRIVE_IBM | devselect | head);

    /* send the read command */
    outb(port + ATA_CMD, ATA_CMD_READ);

	/* wait for the device to be ready */
    for(timeout = 0; timeout < 30000; timeout++)
    {
        status = inb(port + ATA_STATUS);
        if(!(status & ATA_STATUS_BUSY))
            break;
        udelay(1);
    }

    /* get the data */
    buffer = (char *) buf;
    for(i = 0; i < 512; i++)
        buffer[i] = inb(port + ATA_DATA);
}

/*
 * Lists the drives and their partitions on the IDE controllers 
 */
struct drive *find_drives()
{
    struct drive *ret = 0;

    int i, j, k;
    /* the 4 ports that we have to test */
    int ports[4] = {0x1F0, 0x170, 0xF0, 0x70};
    /* and their  control ports */
    int ctrl_ports[4] = {0x3F0, 0x370, 0x2F0, 0x270};
    int port;
    int ctrl_port;
    unsigned char bufsect[512];

    /* test each port */
    for(i = 0; i < 4; i++)
    {
        /* tmp buffers */
        char byte1, byte2;
        unsigned char a = 0, b = 0;

        /* wich port are we gonna use for this iteration ? */
        port = ports[i];
        ctrl_port = ctrl_ports[i];

        /* is our controller present ? */
        byte1 = inb(port + 6);
        byte2 = (byte1 & 0x10) >> 4;

        if(byte2 == 0)
        {
            byte1 |= 0x10;
            outb(port + 6, byte1);
        }

        else
        {
            byte1 = byte1 & 0xEF;
            outb(port + 6, byte1);
        }

        byte1 = inb(port + 6);
        byte1 = (byte1 & 0x10) >> 4;

        if(byte1 != byte2) /* the controller is present */
        {
            /* initialize it */
            outb(ctrl_port + 6, 4);
            udelay(100); 
            outb(ctrl_port + 6, 0);

            /* select the master drive */
            outb(port + ATA_DRIVE, ATA_DRIVE_IBM | ATA_DRIVE_MASTER);
            udelay(100);

            /* get sector count and sector number */
            a = inb(port + ATA_SECTOR_COUNT);
            b = inb(port + ATA_SECTOR_NUMBER);

            if((a == 1) && (b == 1))
            {
                struct drive *d;
                printf("> %d", d);
                d->port = i;

				/* which type of device is this ? */
                a = inb(port + ATA_CYL_LSB);
                b = inb(port + ATA_CYL_MSB);

                if((a == 0) && (b == 0))
                {
                    /* this is an ATA drive */
                    d->type = ATA_DISK_MASTER | ATA_DISK;

                    /* read the partition table */
                    ide_read(0, &bufsect, port, ATA_DISK_MASTER);

                    /* check if we have a valid MBR signature */
                    if((bufsect[510] == 0x55) && (bufsect[511] == 0xAA))
                    {
                        struct partition *part = 0;

                        for(j = 0; j < 4; j++)
                        {
                            unsigned char fs_magic_number = bufsect[450+j*16];

                            if(fs_magic_number != 0)
                            {
                                struct partition *p;
                                p->drive = d;
                                p->fs_type = fs_magic_number;
                                p->no = j;
                                p->next = part;
                                part = p;
                            }
                        }

                        d->parts = part;
                    } 
                } 

                else if((a == 0x14) && (b == 0xEB))
                {
                    /* this is an ATA drive */
                    d->type = ATA_DISK_MASTER | ATAPI_DISK;
                }
            
                d->next = ret;
                ret = d;
                printf("> %d", ret);
            }
        }

        /* the controller is missing, do nothing */
    }

    return ret;
}

void list_drives(struct drive *d)
{
    printf("miaou");
    while(d != 0)
    {
        printf("Found drive, port %d", d->port);

        if(d->type & ATA_DISK_MASTER)
            printf(", master");

        if(d->type & ATA_DISK)
        {
            printf(", ATA\n");

            struct partition *part = d->parts;

            while(part)
            {
                int fs = part->fs_type;
                printf("\tFound partition %d, type %s (%d", part->no,
                    identify_fs(fs), fs);

                part = part->next;
            }
        }

        else if(d->type & ATAPI_DISK)
            printf(", ATAPI\n"); 

        d = d->next;
    }
}