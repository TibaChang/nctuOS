### nctuOS

A tiny OS that used for course OSDI in National Chiao Tung University, Computer Science Dept.

This OS only supports x86

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
