### nctuOS

A tiny OS that used for course OSDI in National Chiao Tung University, Computer Science Dept.

This OS only supports x86

### Lab 5

In this lab, you will learn about process management, basic scheduling and system calls.

In our design, each process contains a page directory so that each of them can have full memory addressing space.

For each process, there are several attributes that are necessary for kernel to manage the process easily, you can reference `kernel/task.h` for more detail. 

**Source Tree**

`kernel/*`: Includes all the file implementation needed by kernel only.
`lib/*`: Includes libraries that should be in user space.
`inc/*`: Header files for user.
`user/*`: Files for user program.
`boot/*`: Files for booting the kernel.

You can leverage `grep` to find out where to fill up to finish this lab.

`$ grep -R TODO .`

To run this kernel

    $ make
    $ make qemu

To debug

    $ make debug

**Modifications to be made**
- `lib/syscall.c` implement the interfaces of system calls for user program to use.
- `kernel/task.h` to open interfaces of implementation of system calls for `kernel/syscall.c`
- `kernel/task.c` implement `task_create`, `task_free`, `sys_kill`, `sys_fork`, and `task_init`
- `kernel/syscall.c` implement system calls
- `kernel/trap_entry.S` implement trap handler interface for system call
- `kernel/mem.c` implement `setupkvm` used by each process creation
- `kernel/sched.c` implement scheduler
- `kernel/timer.c` implement timer_handler to support sleep

### Lab 4

In this lab, you will learn about memory management in x86.

You can leverage `grep` to find out where to fill up to finish this lab.

`$ grep -R TODO .`

To run this kernel

    $ make
    $ qemu -hda kernel.img -monitor stdio

- Modify `kernel/mem.c` to implement the memory management functions
- Modify `kernel/trap.c` and `kernel/trap_entry.S` to setup IDT for pagefault


### Lab 3

In this lab, you will learn about interrupt and segmentation mechanism in x86.

We provided you with keyboard handler, timer handler, and simple VGA driver.

You can leverage `grep` to find out where to fill up to finish this lab.

`$ grep -R TODO .`

To run this kernel

    $ make
    $ qemu -hda kernel.img -monitor stdio

- Modify `boot/boot.S` to setup GDT
- Modify `kernel/trap.c` and `kernel/trap_entry.S` to setup IDT for keyboard and timer
- Modify `kernel/main.c` to uncomment the setup process
- Modify `kernel/shell.c` to support `kerninfo` and `chgcolor`

**After this lab, you should know about how interrupt works and the working flow of GDT & IDT**

### Acknowledgement

This is forked and modified from MIT's Xv6
