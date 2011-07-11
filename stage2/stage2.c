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

/* Macro that tells if the bit 'n' of 'flags' is set to 1 */
#define IS_SET(flags,n) ((flags >> n) & 0x1)

#define reboot() outb(0x64, 0xFE)

/*
 * Entry point, called from the ASM file. Main method of stage2
 */
int main(unsigned long magic, struct multiboot *mboot)
{        
    char *video = (char *)0xB8000;
    char *cmdline;
    unsigned int bootdev, flags;
    int i = 0;
    
    /* Clear screen */
    for(i; i < 160*25; i++)
        *(video + i) = 0;
        
    /* Are you Grub ? */
    if(magic == MULTIBOOT_BOOTLOADER_MAGIC)
    {
        printf("> Booting from a multiboot-compliant bootloader... \n");
        flags = mboot->flags;
        
        /* Read mem info : bit 0 indicates if mem fields are valids */
        if(IS_SET(flags, 0))
        {
            printf("> Lower memory : found %d Kb \n", mboot->mem_lower);
            printf("> Upper memory : found %d Kb \n", mboot->mem_upper);
        } else printf("> Unable to get memory info \n");
        
        /* Where are we booting from ? 
         * Bit 1 indicates if this field is ok */
        if(IS_SET(flags, 1))
        {
            bootdev = mboot->boot_device;
            printf("> Booting from BIOS device %d\n", (bootdev & 0xFF));
        } 
        
        /* Read the kernel command line */
        if(IS_SET(flags, 2))
        {
            cmdline = (char *) mboot->cmd_line;
            printf("> Kernel command line : %s \n", cmdline);
        }
    }
    else
        printf("> Booting from an unknown bootloader \n");
    
    whereami();
    return 0;
}
