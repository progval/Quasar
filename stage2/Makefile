SOURCES=stage2_s.o stage2_c.o printf.o disk.o io.o shell.o interrupts_callback.o
CFLAGS=-nostdlib -fno-builtin -fno-stack-protector
LDFLAGS=-Tlinker.ld
ASFLAGS=-f elf
CC=gcc
AS=nasm

all: $(SOURCES) link

clean:
	rm *.o stage2.bin image-fs/system/stage2.bin

link:
	ld $(LDFLAGS) -o stage2.bin $(SOURCES)

stage2_s.o: stage2.s
	$(AS) $(ASFLAGS) -o stage2_s.o stage2.s
	
interrupts_callback.o: interrupts_callback.s
	$(AS) $(ASFLAGS) -o interrupts_callback.o interrupts_callback.s

stage2_c.o: stage2.c
	$(CC) $(CFLAGS) -c stage2.c -o stage2_c.o
	
.o: .c
	$(CC) $(CFLAGS) -c $^ -o $@
	
image: 
	if [ ! -e image-fs/system ]; then mkdir image-fs/system; fi
	cp stage2.bin image-fs/system/stage2.bin
	grub-mkrescue -o Quasar.iso image-fs/

qemu:
	qemu -boot d -cdrom Quasar.iso
