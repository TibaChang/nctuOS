# Makefile for the simple kernel.
CC	=gcc
AS	=as
LD	=ld
OBJCOPY = objcopy
OBJDUMP = objdump
NM = nm

CFLAGS =  -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin 

# Add debug symbol
CFLAGS += -g

CFLAGS += -I.

#CFLAGS += -DDEBUG_SPINLOCK

OBJDIR = .

CPUS ?= 4

include boot/Makefile
include kernel/Makefile

.PHONY:all
all: boot/boot kernel/system
	dd if=/dev/zero of=$(OBJDIR)/kernel.img count=10000 2>/dev/null
	dd if=$(OBJDIR)/boot/boot of=$(OBJDIR)/kernel.img conv=notrunc 2>/dev/null
	dd if=$(OBJDIR)/kernel/system of=$(OBJDIR)/kernel.img seek=1 conv=notrunc 2>/dev/null

clean:
	rm -rf $(OBJDIR)/boot/*.o $(OBJDIR)/boot/boot.out $(OBJDIR)/boot/boot $(OBJDIR)/boot/boot.asm
	rm -rf $(OBJDIR)/kernel/*.o $(OBJDIR)/kernel/system* kernel.*
	rm -rf $(OBJDIR)/lib/*.o
	rm -rf $(OBJDIR)/user/*.o
	rm -rf $(OBJDIR)/user/*.asm

cscope:
	cscope -Rbqf ./cscope.out

qemu: boot/boot kernel/system
	dd if=/dev/zero of=$(OBJDIR)/kernel.img count=10000 2>/dev/null
	dd if=$(OBJDIR)/boot/boot of=$(OBJDIR)/kernel.img conv=notrunc 2>/dev/null
	dd if=$(OBJDIR)/kernel/system of=$(OBJDIR)/kernel.img seek=1 conv=notrunc 2>/dev/null
	qemu-system-i386 -hda kernel.img -monitor stdio -smp $(CPUS)
gdb: boot/boot kernel/system
	dd if=/dev/zero of=$(OBJDIR)/kernel.img count=10000 2>/dev/null
	dd if=$(OBJDIR)/boot/boot of=$(OBJDIR)/kernel.img conv=notrunc 2>/dev/null
	dd if=$(OBJDIR)/kernel/system of=$(OBJDIR)/kernel.img seek=1 conv=notrunc 2>/dev/null
	qemu-system-i386 -hda kernel.img -monitor stdio -s -S -smp $(CPUS)

