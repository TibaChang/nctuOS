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


### Acknowledgement

This is forked and modified from MIT's Xv6
