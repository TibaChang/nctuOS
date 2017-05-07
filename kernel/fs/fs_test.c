#include "ff.h"

/* Low level I/O API test */
void fs_test()
{
    FATFS fs;      /* File system object (volume work area) */
    FIL fil;       /* File object */
    FRESULT res;   /* API result code */
    UINT bw;       /* Bytes written */

    /* Register work area (do not care about error) */
    res = f_mount(&fs, "/", 1);
    if (res != 0)
    {
        printk("Mount failed %d, try do mkfs.\n", res);
        /* Create FAT volume with default cluster size */
        res = f_mkfs("/", 0, 0);
        res = f_mount(&fs, "/", 1); //Remount
    }

    /* Create a file as new */
    res = f_open(&fil, "hello.txt", FA_CREATE_ALWAYS | FA_WRITE);
    
    if (res != 0)
        printk("Open failed %d\n", res);

    /* Write a message */
    f_write(&fil, "Hello, World!\r\n", 15, &bw);

    /* Close the file */
    f_close(&fil);

    /* Unregister work area */
    f_mount(0, "", 0);
}
