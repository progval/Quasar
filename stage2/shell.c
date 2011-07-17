/*
 * shell.c
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
 
#include "stage2.h"
 
int keyboard_enabled = 0;
char buf[256];
int offset = 0;

void prompt()
{
    for(offset = 0; offset < 256; offset++)
        buf[offset] = 0;
    offset = 0;

    keyboard_enabled = 1;
    printf("\n> ");
}

static int strcmp(const char *s1, const char *s2)
{	
	while(*s1 && *s2)
	{
		if(*s1 > *s2)
			return 1;
		if(*s1 < *s2)
			return -1;
		s1++;
		s2++;
	}
	
	/* we are at the end of 1 str verify if they are both at end */
	if(*s1 == 0 && *s2 == 0)
		return 0;
	else if(*s1 == 0)
		return -1;
	else if(*s2 == 0)
		return 1;
}
		

void run()
{
    int i = 0, space = 0;
    char command[256] = {0};
    char args[256] = {0};
   
    while(buf[i])
    {
        if(buf[i] == ' ' && (space == 0))
			space = i+1;
        else
        {
			if(space == 0)
				command[i] = buf[i];  
			else
				args[i-space] = buf[i];
		}
        i++;
    }
    
    command[i] = 0;
    
    if(strcmp(command, "reboot") == 0)
    {
		printf("Rebooting... \n");
		reboot();
	}
	else
		printf("Unknown command : \"%s\" \n", command);
    
    prompt();
}

unsigned char kbdmap[] = {
	0x1B, 0x1B, 0x1B, 0x1B,	/*      esc     (0x01)  */
	'1', '!', '1', '1',
	'2', '@', '2', '2',
	'3', '#', '3', '3',
	'4', '$', '4', '4',
	'5', '%', '5', '5',
	'6', '^', '6', '6',
	'7', '&', '7', '7',
	'8', '*', '8', '8',
	'9', '(', '9', '9',
	'0', ')', '0', '0',
	'-', '_', '-', '-',
	'=', '+', '=', '=',
	0x08, 0x08, 0x7F, 0x08,	/*      backspace       */
	0x09, 0x09, 0x09, 0x09,	/*      tab     */
	'q', 'Q', 'q', 'q',
	'w', 'W', 'w', 'w',
	'e', 'E', 'e', 'e',
	'r', 'R', 'r', 'r',
	't', 'T', 't', 't',
	'y', 'Y', 'y', 'y',
	'u', 'U', 'u', 'u',
	'i', 'I', 'i', 'i',
	'o', 'O', 'o', 'o',
	'p', 'P', 'p', 'p',
	'[', '{', '[', '[',
	']', '}', ']', ']',
	0x0A, 0x0A, 0x0A, 0x0A,	/*      enter   */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      ctrl    */
	'a', 'A', 'a', 'a',
	's', 'S', 's', 's',
	'd', 'D', 'd', 'd',
	'f', 'F', 'f', 'f',
	'g', 'G', 'g', 'g',
	'h', 'H', 'h', 'h',
	'j', 'J', 'j', 'j',
	'k', 'K', 'k', 'k',
	'l', 'L', 'l', 'l',
	';', ':', ';', ';',
	0x27, 0x22, 0x27, 0x27,	/*      '"      */
	'`', '~', '`', '`',	/*      `~      */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      Lshift  (0x2a)  */
	'\\', '|', '\\', '\\',
	'z', 'Z', 'z', 'z',
	'x', 'X', 'x', 'x',
	'c', 'C', 'c', 'c',
	'v', 'V', 'v', 'v',
	'b', 'B', 'b', 'b',
	'n', 'N', 'n', 'n',
	'm', 'M', 'm', 'm',
	0x2C, 0x3C, 0x2C, 0x2C,	/*      ,<      */
	0x2E, 0x3E, 0x2E, 0x2E,	/*      .>      */
	0x2F, 0x3F, 0x2F, 0x2F,	/*      /?      */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      Rshift  (0x36)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x37)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x38)  */
	' ', ' ', ' ', ' ',	/*      space   */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x3a)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x3b)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x3c)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x3d)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x3e)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x3f)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x40)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x41)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x42)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x43)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x44)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x45)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x46)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x47)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x48)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x49)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x4a)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x4b)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x4c)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x4d)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x4e)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x4f)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x50)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x51)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x52)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x53)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x54)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x55)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x56)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x57)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x58)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x59)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x5a)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x5b)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x5c)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x5d)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x5e)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x5f)  */
	0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x60)  */
	0xFF, 0xFF, 0xFF, 0xFF	/*      (0x61)  */
};

void c_callback_kbd()
{
    unsigned char i;
	static int shift_enable;
	static int alt_enable;
	static int ctrl_enable;
	
    do {
	    i = inb(0x64);
    } while ((i & 0x01) == 0);
    i = inb(0x60);
    i--;
    
    if(keyboard_enabled)
    {
	    if (i < 0x80) {
		    switch (i) {
		    case 0x29:
			    shift_enable = 1;
			    break;
		    case 0x35:
			    shift_enable = 1;
			    break;
		    case 0x1C:
			    ctrl_enable = 1;
			    break;
		    case 0x37:
			    alt_enable = 1;
			    break;
			case 0x0d: 
			    if(offset != 0)
			    {
			        putcar(0x08);
			        offset--;
			        buf[offset] = 0;
			    }
			    break;
			case 0x1B:
				printf("\n");
			    run();
			    break;
		    default:
			    putcar(kbdmap
			           [i * 4 + shift_enable]);
			    buf[offset] = kbdmap
			           [i * 4 + shift_enable];
			    offset++;
		    }
	    } else {	
		    i -= 0x80;
		    switch (i) {
		    case 0x29:
			    shift_enable = 0;
			    break;
		    case 0x35:
			    shift_enable = 0;
			    break;
		    case 0x1C:
			    ctrl_enable = 0;
			    break;
		    case 0x37:
			    alt_enable = 0;
			    break;
		    }
	    }
    }
}
