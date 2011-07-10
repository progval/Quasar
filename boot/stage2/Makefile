SOURCES=stage2_s.o stage2_c.o printf.o disk.o io.o
CFLAGS=-nostdlib -fno-builtin -fno-stack-protector
LDFLAGS=-Tlinker.ld
ASFLAGS=-f elf
CC=gcc
AS=nasm

all: $(SOURCES) link

clean:
	rm *.o stage2.bin

link:
	ld $(LDFLAGS) -o stage2.bin $(SOURCES)

stage2_s.o: stage2.s
	$(AS) $(ASFLAGS) -o stage2_s.o stage2.s

stage2_c.o: stage2.c
	$(CC) $(CFLAGS) -c stage2.c -o stage2_c.o
	
.o: .c
	$(CC) $(CFLAGS) -c $^ -o $@
	
image: update_image.sh
	bash update_image.sh

qemu:
	qemu -boot a -fda floppy.img -hda c.img