# Define variables
include ../../../../mk/toolchain.mk
ARCH = -march=rv32izicsr
LINKER_SCRIPT = linker.ld

EMU ?= ../../../../build/rv32emu

# Correct OP usage: make sure -O1 is properly set
OP := O1
CFLAGS = -g -$(OP) -march=rv32i_zicsr
AFLAGS = -g $(ARCH)

CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
OBJDUMP = $(CROSS_COMPILE)objdump

SHELL := /bin/bash
CURRENT_DIR = $(shell pwd)

# Assuming OBJS is meant to be the name of the object file(s)
OBJS = hanoi.o hanoi_c.o

.PHONY: all run dump clean

all: $(OBJS)

# Rule for compiling .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) $< -o $@ -c
	
%.o: %.S
	$(AS) $(AFLAGS) $< -o $@

# Rule for dumping disassembled output
dump: clean $(OBJS)
	@for obj in $(OBJS); do \
		$(OBJDUMP) -d $$obj > $${obj:=}_$(OP).txt; \
	done

clean:
	rm -f *.o
# Clean rule
clean_all:
	rm -f *.o *.txt

