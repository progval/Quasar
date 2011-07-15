/*
 * io.c
 * Copyright (C) 2011 Florent Revest <florent.revest666@gmail.com>
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
 * Implementation of a few I/O functions declared in stage2.h
 * Makes the ASM I/O instructions outb, inb, etc. usable from our C code
 * Those functions are marked as inline for improved performance
 */
 
/*
 * outb : send the byte 'value' to the given 'port'
 */
inline void outb(unsigned short port, unsigned char value)
{
    asm volatile("outb %%al, %%dx" :: "d" (port), "a" (value));
}

/*
 * Read a byte from the given 'port' 
 */
inline unsigned char inb(unsigned short port)
{
    unsigned char ret;
    asm volatile("inb %%dx,%%al":"=a" (ret):"d" (port));
    return ret;
}

/*
 * Read a word (16 bits) from the given 'port'
 */
inline unsigned short inw(unsigned short port)
{
    unsigned short ret;
    asm volatile("inw %%dx,%%ax":"=a" (ret):"d" (port));
    return ret;
}

void putcar(unsigned char c)
{
    static int x = 0, y = 0;
    unsigned char *video, *tmp;
    video = (unsigned char *)(0xB8000 + 2*x + 160*y);

    switch(c) {
    case '\n':
        x = 0;
        y++;
    break;

    case 8:
        if(x) 
        {
            *(video+1) = 0x0;
            x--;
        }
    break;

    case 9:
        x = x + 8 - (x % 8);
    break;

    case '\r':
        x = 0;
    break;

    default:
        *video = c;
        *(video + 1) = 0x07;
        x++;
        if(x > 79)
        {
            x = 0;
            y++;
        }
    break;
    }

    if(y > 24)
    {
        for(video = (unsigned char *) 0xB8000; 
			video < (unsigned char *)0xB8000 + 160*25; 
			video+=2)
        {
            tmp = (unsigned char*) (video + 160*(y - 24));
            
            if(tmp < (unsigned char*) (0xB8000 + 160*25))
            {
                *video = *tmp;
                *(video+1) = *(tmp + 1);
            }
            else
            {
                *video = 0;
                *(video + 1) = 0x07;
            }
        }

        y -= (y - 24);
        if(x < 0)
            x = 0;
    }
}
