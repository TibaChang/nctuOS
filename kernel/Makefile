
GCC_LIB := $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

KERN_LDFLAGS = -T kernel/kern.ld



KERN_OBJS = kernel/entry.o \
	kernel/main.o \
	kernel/picirq.o \
	kernel/kbd.o \
	kernel/screen.o \
	kernel/trap.o \
	kernel/trap_entry.o \
	kernel/printf.o \
	kernel/mem.o \
	kernel/entrypgdir.o \
	kernel/assert.o \
	kernel/kclock.o \
	kernel/timer.o \
	kernel/task.o \
	kernel/syscall.o \
	kernel/sched.o \
	kernel/drv/disk.o \
	kernel/spinlock.o \
	kernel/lapic.o \
	kernel/mpentry.o \
	kernel/mpconfig.o
#lab7
CFLAGS += -Ikernel/fs -Ikernel/fs/fat

FS_SRC = kernel/fs
FS_OBJS = kernel/fs/fat/ff.o \
        kernel/fs/diskio.o \
        kernel/fs/fs_syscall.o \
        kernel/fs/fs_ops.o \
        kernel/fs/fs.o \
        kernel/fs/fs_test.o

ULIB = lib/string.o lib/printf.o lib/printfmt.o lib/readline.o lib/console.o lib/syscall.o

UPROG = user/shell.o user/main.o

kernel/drv/%.o: kernel/drv/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

kernel/%.o: kernel/%.c
	$(CC) $(CFLAGS) -Os -c -o $@ $<

kernel/%.o: kernel/%.S
	$(CC) $(CFLAGS) -c -o $@ $<

lib/%.o: lib/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

user/%.o: user/%.c
	$(CC) $(CFLAGS) -c -o $@ $<
	$(OBJDUMP) -S $@ > $@.asm

kernel/fs/%.o: kernel/fs/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

kernel/fs/fat/%.o: kernel/fs/fat/%.c
	$(CC) $(CFLAGS) -c -o $@ $<


kernel/system: $(KERN_OBJS) $(DRV) $(ULIB) $(UPROG) $(FS_OBJS)
	@echo + ld kernel/system

	$(LD) $(LDFLAGS) $(KERN_LDFLAGS) $(KERN_OBJS) $(FS_OBJS) $(GCC_LIB) -o $@

	$(OBJDUMP) -S $@ > $@.asm
	$(NM) -n $@ > $@.sym
