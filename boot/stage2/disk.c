#include "stage2.h"

static inline void udelay(unsigned int delay)
{
    int i;
    for(i = 0; i < (delay*1000); i++)
    {
        i++;
        i--;
    }
}

static char *identify_fs(unsigned char magic_number)
{
    switch(magic_number)
    {
        case NO_FS:
            return "Empty partition";
        case FS_EXT:
            return "Linux EXT";
            break;
        default:
            return "Unknown FS";
            break;
    }
}
            

static void ide_read(long sector, void *buf, unsigned short port, char master)
{
    unsigned char cyl_low, cyl_high, sect, head, status;
    int devselect, i, timeout;
    short *buffer;
    
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
    
    for(timeout = 0; timeout < 30000; timeout++)
    {
        status = inb(port + ATA_STATUS);
        if(!(status & ATA_STATUS_BUSY))
            break;
        udelay(1);
    }
    
    buffer = (short *) buf;
    for(i = 0; i < 256; i++)
        buffer[i] = inw(port + ATA_DATA);
}


void whereami()
{
    int i, j, k;
    /* the 4 ports that we have to test */
    int ports[4] = {0x1F0, 0x170, 0xF0, 0x70};
    /* and their  control ports */
    int ctrl_ports[4] = {0x3F0, 0x370, 0x2F0, 0x270};
    int port;
    int ctrl_port;
    unsigned char bufsect[512];

    printf("> Searching for drives and partitions... \n");

    /* test each port */
    for(i = 0; i < 4; i++)
    {
        printf("> Trying port %d \n", i);
        
        /* tmp buffers */
        char byte1, byte2;
        unsigned char a = 0, b = 0;
    
        /* wich port are we gonna use for this iteration ? */
        port = ports[i];
        ctrl_port = ctrl_ports[i];
        
        /* is our controller present ? */
        /* je précise que je n'ai rien compris à comment on fait
           mais qu'apparemment on fait comme ça alors c'est cool */
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
            printf(">     Found controller \n");
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
                a = inb(port + ATA_CYL_LSB);
                b = inb(port + ATA_CYL_MSB);
                
                if((a == 0) && (b == 0))
                {
                    /* this is an ATA drive */
                    printf(">     Found ATA drive. Looking for partitions \n");
                    ide_read(0, &bufsect, port, ATA_DISK_MASTER);
                    if((bufsect[510] == 0x55) && (bufsect[511] == 0xAA))
                    {
                        for(j = 0; j < 4; j++)
                        {
                            char *buf = identify_fs(bufsect[450 + j * 16]);
                            printf(">           Found partition type %s\n",buf);
                        }
                    }
                }
                
                else if((a == 0x14) && (b == 0xEB))
                    printf(">     Found ATAPI drive \n");
            }
        }
    }
}