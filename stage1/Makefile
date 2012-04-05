AS=nasm
ASFLAGS=-f bin

all: stage1.bin

stage1.bin: stage1.s
	$(AS) $(ASFLAGS) -o $@ $^

clean:
	rm stage1.bin
