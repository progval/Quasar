AS = nasm
ASFLAGS = -f bin

all: loader

loader: loader.s
	$(AS) $(ASFLAGS) -o $@ $^

clean:
	rm loader
