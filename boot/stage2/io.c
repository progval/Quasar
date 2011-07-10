inline void outb(unsigned short port, unsigned char value)
{
    asm volatile("outb %%al, %%dx" :: "d" (port), "a" (value));
}

inline unsigned char inb(unsigned short port)
{
    unsigned char ret;
    asm volatile("inb %%dx,%%al":"=a" (ret):"d" (port));
    return ret;
}

inline unsigned short inw(unsigned short port)
{
    unsigned short ret;
    asm volatile("inw %%dx,%%ax":"=a" (ret):"d" (port));
    return ret;
}