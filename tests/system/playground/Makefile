include ../../../mk/toolchain.mk

ARCH = -march=rv32izicsr
LINKER_SCRIPT = linker.ld

EMU ?= ../../../build/rv32emu

OP := -O1
AFLAGS = -g $(ARCH)
CFLAGS = -g $(OP) -march=rv32i_zicsr
LDFLAGS = -T $(LINKER_SCRIPT)
EXEC = test.elf

CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
LD = $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump

SHELL := /bin/bash
CURRENT_DIR = $(shell pwd)
RISCV_DIR := $(HOME)/riscv-none-elf-gcc
SETENV := $(HOME)/riscv-none-elf-gcc/setenv
export PATH := $(RISCV_DIR)/bin:$(PATH)

OBJS = start.o main.o perfcounter.o chacha20_asm.o uf8.o hanoi.o hanoi_c.o rsqrt_c.o rsqrt.o print.o

.PHONY: all run dump clean

all: $(EXEC)

$(EXEC): $(OBJS) $(LINKER_SCRIPT)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.S
	$(AS) $(AFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@ -c

run: $(EXEC)
	@test -f $(EMU) || (echo "Error: $(EMU) not found" && exit 1)
	@grep -q "ENABLE_ELF_LOADER=1" ../../../build/.config || (echo "Error: ENABLE_ELF_LOADER=1 not set" && exit 1)
	@grep -q "ENABLE_SYSTEM=1" ../../../build/.config || (echo "Error: ENABLE_SYSTEM=1 not set" && exit 1)
	$(EMU) $<

dump: $(EXEC)
	$(OBJDUMP) -Ds $< | less

disassemble:
	$(OBJDUMP) -d test.elf > test_disassemble.txt

clean:
	rm -f $(EXEC) $(OBJS)
