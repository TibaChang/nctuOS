/* Copy from MIT 6.828 course resource
*  Reference: http://pdos.csail.mit.edu/6.828/2012/
*/

/* Support for reading the NVRAM from the real-time clock. */

#include <inc/x86.h>

#include <kernel/kclock.h>


unsigned
mc146818_read(unsigned reg)
{
	outb(IO_RTC, reg);
	return inb(IO_RTC+1);
}

void
mc146818_write(unsigned reg, unsigned datum)
{
	outb(IO_RTC, reg);
	outb(IO_RTC+1, datum);
}
