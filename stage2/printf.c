/*
 * printf.c
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
 * Re-implementation of the C stdlib printf function.
 * Also implements internally the itoa and strlen functions of the
 * cstdlib as printf needs them.
 */
 
#include <stdarg.h>

int x = 0, y = 0;

static int strlen(char *s)
{
    int ret = 0;
    while(*s++)
        ret++;
    return ret;
}

static void itoa (char *buf, int d, int base)
{
    char *p = buf;
    char *p1, *p2;
    unsigned long ud = d;

    /* If %d is specified and D is minus, put `-' in the head. */
    if (base == 10 && d < 0)
    {
        *p++ = '-';
        buf++;
        ud = -d;
    }
     
    /* Divide UD by DIVISOR until UD == 0. */
    do
    {
        int remainder = ud % base;
     
        *p++ = (remainder < 10) ? remainder+'0' : remainder+'a'-10;
    }
    while (ud /= base);
     
    /* Terminate BUF. */
    *p = 0;
     
    /* Reverse BUF. */
    p1 = buf;
    p2 = p - 1;
    while (p1 < p2)
    {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
}


static void putcar(unsigned char c)
{
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
        

void printf(char *s, ...)
{
    va_list ap;
    char buf[16];
    int i, j, size, buflen, neg;
    unsigned char c;
    int ival;
    unsigned int uival;
    
    va_start(ap, s);
    
    while((c = *s++))
    {
        size = 0;
        neg = 0;
        
        if(c == 0) /* str end */
            break;
            
        else if(c == '%')
        {
            c = *s++;
            
            if(c >= '0' && c <= '9')
            {
                size = c - '0';
                c = *s++;
            }
            
            if(c == 'd')
            {
                ival = va_arg(ap, int);
                
                /* is our number negative ? */
                if(ival < 0)
                {
                    /* yes, store it's absolute value */
                    uival = 0 - ival;
                    neg++;
                }
                else
                    uival = ival;
                    
                itoa(buf, uival, 10);
                buflen = strlen(buf);
                
                if(buflen < size)
                {
                    for(i = size, j = buflen; i >= 0; i--, j--)
                        buf[i] = (j >= 0) ? buf[j] : '0';
                }        
                
                if(neg)
                    printf("-%s", buf);
                else
                    printf("%s", buf);
            }
            
            else if(c == 'u')
            {
                uival = va_arg(ap, int);
                itoa(buf, uival, 10);
                    
                buflen = strlen(buf);
                if(buflen < size)
                {
                    for(i = size, j = buflen; i >= 0; i--, j--)
                        buf[i] = (j >= 0) ? buf[j] : '0';
                }
                
                printf(buf);
            }
            
            else if(c == 'x' || c == 'X')
            {
                uival = va_arg(ap, int);
                itoa(buf, uival, 16);
                
                buflen = strlen(buf);
                if(buflen < size)
                {
                    for(i = size, j = buflen; i >= 0; i--, j--)
                        buf[i] = (j >= 0) ? buf[j] : '0';
                }
                    
                printf("0x%s", buf);
            }        
            
            else if(c == 's')
                printf((char *) va_arg(ap, int));
                
        }
        
        else
            putcar(c);
            
    }
    
    return;
}
