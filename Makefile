SRC := $(wildcard boot/*.c kernel/*.c)
ASM := $(wildcard boot/*.S trap/*.S)
TMP := $(SRC:.c=.o) $(ASM:.S=.o)
OBJ := $(TMP:boot/entry.o=)

ifndef CPUS
CPUS := 1
endif

BLKCOUNT := 1

QEMU := qemu-system-riscv64
ifndef TOOLPREFIX
TOOLPREFIX := $(shell if riscv64-unknown-elf-objdump -i 2>&1 | grep 'elf64-big' >/dev/null 2>&1; \
	then echo 'riscv64-unknown-elf-'; \
	elif riscv64-linux-gnu-objdump -i 2>&1 | grep 'elf64-big' >/dev/null 2>&1; \
	then echo 'riscv64-linux-gnu-'; \
	elif riscv64-unknown-linux-gnu-objdump -i 2>&1 | grep 'elf64-big' >/dev/null 2>&1; \
	then echo 'riscv64-unknown-linux-gnu-'; \
	else echo "***" 1>&2; \
	echo "*** Error: Couldn't find a riscv64 version of GCC/binutils." 1>&2; \
	echo "*** To turn off this error, run 'gmake TOOLPREFIX= ...'." 1>&2; \
	echo "***" 1>&2; exit 1; fi)
endif


QEMUOPTS = -machine virt -bios none -kernel kernel.bin -m 128M -smp $(CPUS) -nographic
QEMUOPTS += -global virtio-mmio.force-legacy=false
QEMUOPTS += -drive file=vhd,if=none,format=raw,id=x0
QEMUOPTS += -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gas
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump

CFLAGS = -Wall -Werror -O -fno-omit-frame-pointer -ggdb -gdwarf-2
CFLAGS += -MD
CFLAGS += -mcmodel=medany
CFLAGS += -ffreestanding -fno-common -nostdlib -mno-relax
CFLAGS += -I.
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)

# build: kernel.bin

qemu: kernel.bin vhd
	$(QEMU) $(QEMUOPTS) -s -S

# Do `make gdb` separately with `make qemu` otherwie ctrl+c would terminate qemu immediately
gdb:
	gdb -ex "target extended-remote localhost:1234" \
							-ex "symbol-file kernel.o"

vhd:
	dd bs=1M if=/dev/zero of=$@ count=$(BLKCOUNT)


kernel.bin: kernel.o
	$(OBJCOPY) $< $@ -O binary

kernel.o: boot/entry.o $(OBJ)
	$(LD) -Tlink.ld -o $@ $^ 

%.o : %.c
	$(CC) -c $(CFLAGS) -o $@ $< -g

%.o : %.s
	$(AS) -o $@ $< -g

clean:
	@rm kernel.bin *.o vhd 2>/dev/null || :
	@find . -name \*.o -type f -delete
	@find . -name \*.d -type f -delete
