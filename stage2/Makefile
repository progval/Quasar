SOURCES=stage2_s.o stage2_c.o printf.o disk.o io.o shell.o interrupts_callback.o
CFLAGS=-nostdlib -fno-builtin -fno-stack-protector -m32
LDFLAGS=--oformat binary -melf_i386
ASFLAGS=-f elf
AS=nasm

all: stage2.bin

stage2.bin: $(SOURCES)
	$(LD) $(LDFLAGS) -o $@ $^ -e start

stage2_s.o: stage2.s
	$(AS) $(ASFLAGS) -o $@ $^
	
interrupts_callback.o: interrupts_callback.s
	$(AS) $(ASFLAGS) -o $@ $^

stage2_c.o: stage2.c
	$(CC) $(CFLAGS) -c $^ -o $@
	
.o: .c
	$(CC) $(CFLAGS) -c $^ -o $@
	
qemu:
	qemu-system-i386 -boot d -cdrom Quasar.iso

clean:
	rm *.o stage2.bin 
