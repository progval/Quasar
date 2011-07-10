/*
 * stage2.c
 * Copyright (C) 2011 All Rights Reserved © ® ™ Quasar Lab. \o/
 * loading part of stage2
 * The stage2's job is to get multiboot info, load and jump to the kernel module
 */

#include "stage2.h"

/* Macro that tells if the bit 'n' of 'flags' is set to 1 */
#define IS_SET(flags,n) ((flags >> n) & 0x1)

/*
 * Entry point, called from the ASM file. Main method of stage2
 */
int main(unsigned long magic, struct multiboot *mboot)
{        
    char *video = (char *)0xB8000;
    char *cmdline;
    unsigned int bootdev, flags;
    int i = 0;
    
    /* clear screen */
    for(i; i < 160*25; i++)
        *(video + i) = 0;
        
    /* Are you Grub ? */
    if(magic == MULTIBOOT_BOOTLOADER_MAGIC)
    {
        printf("> Booting from a multiboot-compliant bootloader... \n");
        flags = mboot->flags;
        
        /* read mem info : bit 0 indicates if mem fields are valids */
        if(IS_SET(flags,0))
        {
            printf("> Lower memory : found %d Kb \n", mboot->mem_lower);
            printf("> Upper memory : found %d Kb \n", mboot->mem_upper);
        } else printf("> Unable to get memory info \n");
        
        /* where are we booting from ? bit 1 indicates if this field is ok */
        if(IS_SET(flags,1))
        {
            bootdev = mboot->boot_device;
            printf("> Booting from BIOS device %d \n", (bootdev & 0xFF));
        } 
        
        /* Read the kernel command line */
        if(IS_SET(flags,2))
        {
            cmdline = (char *) mboot->cmd_line;
            printf("> Kernel command line : %s \n", cmdline);
        }
        
        /* check if we have VESA */
        if(IS_SET(flags,11))
            printf("GNAGNAGNA VESA \n");
        else
            printf("pas gnagnagna :'( \n");
    }
    else
        printf("> Booting from a unknown bootloader \n");
    
    whereami();
    
    return 0;
}
