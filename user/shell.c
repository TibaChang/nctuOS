#include <inc/syscall.h>
#include <inc/stdio.h>
#include <inc/string.h>
#include <inc/shell.h>
#include <inc/assert.h> 

char hist[SHELL_HIST_MAX][BUF_LEN];

int hist_head;
int hist_tail;
int hist_curr;

/* TODO: Lab7, Please add and implement "ls", "touch" and "rm" command */


/*  Prototypes  */
int mon_help(int argc, char **argv);
int mem_stat(int argc, char **argv);
int print_tick(int argc, char **argv);
int chgcolor(int argc, char **argv);
int forktest(int argc, char **argv);
int filetest(int argc, char **argv);
int fs_seek_test(int argc, char **argv);
int fs_speed_test(int argc, char **argv);
int filetest2(int argc, char **argv);
int filetest3(int argc, char **argv);
int filetest4(int argc, char **argv);
int filetest5(int argc, char **argv);
int spinlocktest(int argc, char **argv);


struct Command commands[] = {
  { "help", "Display this list of commands", mon_help },
  { "mem_stat", "Show current usage of physical memory", mem_stat },
  { "print_tick", "Display system tick", print_tick },
  { "chgcolor", "Change screen text color", chgcolor },
  { "forktest", "Test functionality of fork()", forktest },
  { "filetest", "Test create file", filetest },
  { "fs_seek_test", "Test seek file", fs_seek_test },
  { "fs_speed_test", "Test R/W speed", fs_speed_test},
  { "filetest2", "Open test", filetest2},
  { "filetest3", "Laqrge block test", filetest3},
  { "filetest4", "Error test", filetest4},
  { "filetest5", "unlink test", filetest5},
  { "spinlocktest", "Test spinlock", spinlocktest }
};
const int NCOMMANDS = (sizeof(commands)/sizeof(commands[0]));

int mem_stat(int argc, char **argv)
{
  cprintf("%-10s MEM_STAT %10s\n", "--------", "--------");
  cprintf("Used: %18d pages\n", get_num_used_page());
  cprintf("Free: %18d pages\n", get_num_free_page());
  return 0;
}

int mon_help(int argc, char **argv)
{
  int i;

  for (i = 0; i < NCOMMANDS; i++)
    cprintf("%s - %s\n", commands[i].name, commands[i].desc);
  return 0;
}

int print_tick(int argc, char **argv)
{
  cprintf("Now tick = %d\n", get_ticks());
  return 0;
}

int chgcolor(int argc, char **argv)
{
  if (argc > 1)
  {
    char fore = argv[1][0] - '0';
    settextcolor(fore, 0);
    cprintf("Change color %d!\n", fore);
  }
  else
  {
    cprintf("No input text color!\n");
  }
  return 0;
}

#define WHITESPACE "\t\r\n "
#define MAXARGS 16

static int runcmd(char *buf)
{
  int argc;
  char *argv[MAXARGS];
  int i;

  // Parse the command buffer into whitespace-separated arguments
  argc = 0;
  argv[argc] = 0;
  while (1) {
    // gobble whitespace
    while (*buf && strchr(WHITESPACE, *buf))
      *buf++ = 0;
    if (*buf == 0)
      break;

    // save and scan past next arg
    if (argc == MAXARGS-1) {
      cprintf("Too many arguments (max %d)\n", MAXARGS);
      return 0;
    }
    argv[argc++] = buf;
    while (*buf && !strchr(WHITESPACE, *buf))
      buf++;
  }
  argv[argc] = 0;

  // Lookup and invoke the command
  if (argc == 0)
    return 0;
  for (i = 0; i < NCOMMANDS; i++) {
    if (strcmp(argv[0], commands[i].name) == 0)
      return commands[i].func(argc, argv);
  }
  cprintf("Unknown command '%s'\n", argv[0]);
  return 0;
}



void task_job()
{
	int pid = 0;
	int cid = 0;
	int i;

	pid = getpid();
	cid = getcid();
	for (i = 0; i < 10; i++)
	{
		cprintf("Pid=%d, Cid=%d, now=%d\n", pid, cid, i);
		sleep(100);
	}
}

int forktest(int argc, char **argv)
{
  /* Below code is running on user mode */
  if (!fork())
  {

    /*Child*/
    task_job();
    if (fork())
      task_job();
    else
    {
      if (fork())
        task_job();
      else
        if (fork())
          task_job();
        else
          task_job();
    }
    /* task recycle */
    kill_self();
  }
  return 0;
}

int spinlocktest(int argc, char **argv)
{
  /* Below code is running on user mode */
  if (!fork())
  {
    /*Child*/
    fork();
    fork();
    fork();
    sleep(500);
    cprintf("Pid=%d, Cid=%d\n", getpid(), getcid());
    /* task recycle */
    kill_self();
  }
  return 0;
}
#define BUFSIZE 128
int filetest(int argc, char **argv)
{
    int fd = -1;
    int ret;
    char *test_str = "This is the last LAB!! Yah!";
    char buf[BUFSIZE] = {0};

    if ((fd = open("hello.txt", O_WRONLY | O_CREAT | O_TRUNC, 0)) >= 0)
    {
        cprintf("Open successed!\n");

        ret = write(fd, test_str, strlen(test_str));
        if (ret > 0)
            cprintf("Write %d bytes!\n", ret);
        else
            cprintf("Write failed %d!\n", ret);

        close(fd);
    }
    else
        cprintf("Open failed! %d\n", fd);

    if ((fd = open("hello.txt", O_RDONLY, 0)) >= 0)
    {
        cprintf("Open successed!\n");

        ret = read(fd, buf, BUFSIZE);
        if (ret > 0)
            cprintf("Read \"%s\"\n", buf);
        else
            cprintf("Read failed %d!\n", ret);

        close(fd);
    }
    else
        cprintf("Open failed!\n");
    return 0;
}
int filetest2(int argc, char **argv)
{
    int fd[20], i;
    for (i = 0; i < 20; i++)
    {
        fd[i] =  open("hello.txt", O_WRONLY | O_CREAT | O_TRUNC, 0);
        if (fd[i] < 0)
            cprintf("Open error, %d\n", fd[i]);
        else
            cprintf("Open successed, %d\n", i);
    }
    for (i = 0; i < 20; i++)
    {
        if (fd[i] >= 0)
            close(fd[i]);
    }
    return 0;
}

#define uassert(x)    \
  do { if (!(x)) {cprintf("assertion failed: %s", #x); return 0;}} while (0)
#define LARGE_SIZE 4000
int filetest3(int argc, char **argv)
{
    int fd, i, ret;
    char larg_buf[LARGE_SIZE] = {0};
    fd =  open("large.txt", O_RDWR | O_CREAT | O_TRUNC, 0);
    if (fd >= 0)
    {
        for (i = 0; i < LARGE_SIZE; i++)
        {
            larg_buf[i] = i & 0xFF;
        }
        ret = write(fd, larg_buf, LARGE_SIZE);
        uassert(ret == LARGE_SIZE);
    }
    else
    {
        cprintf("Open error, %d\n", fd);
        return -1;
    }
    ret = lseek(fd, 0, SEEK_SET); //seek to file begin
    uassert(ret == 0);
    for (i = 0; i < LARGE_SIZE; i++)
    {
        larg_buf[i] = 0;
    }
    ret = read(fd, larg_buf, LARGE_SIZE);
    uassert(ret == LARGE_SIZE);
    for (i = 0; i < LARGE_SIZE; i++)
    {
        //uassert(larg_buf[i] == i&0xFF);
        if ((larg_buf[i]&0xFF) != (i& 0xFF))
        {
            cprintf("Failed at %d, read %x but want %d\n", i, larg_buf[i], i&0xFF);
            return -1;
        }
    }
    close(fd);
    cprintf("Large file test successed!\n");
    return 0;
}
/* Test error code.*/
int filetest4(int argc, char **argv)
{
    int fd, ret, i;
    char buf[BUFSIZE] = {0};

    for (i = 0; i < BUFSIZE; i++)
    {
        buf[i] = i & 0xFF;
    }

    fd = open("test4", O_WRONLY, 0);
    uassert(fd == -STATUS_ENOENT);

    fd = open("test4", O_WRONLY | O_CREAT | O_TRUNC, 0);
    uassert(fd >= STATUS_OK);

    ret = close(100);
    uassert(ret == -STATUS_EINVAL);

    ret = close(fd);
    uassert(ret == STATUS_OK);

    fd = open("test4", O_WRONLY | O_CREAT, 0);
    uassert(fd == -STATUS_EEXIST);

    fd = open("test4", O_RDWR | O_CREAT | O_TRUNC, 0);
    uassert(fd >= STATUS_OK);

    ret = write(fd, 0, -1);
    uassert(ret == -STATUS_EINVAL);

    ret = write(100, buf, BUFSIZE);
    uassert(ret == -STATUS_EBADF);

    ret = read(fd, 0, BUFSIZE);
    uassert(ret == -STATUS_EINVAL);

    ret = read(100, buf, BUFSIZE);
    uassert(ret == -STATUS_EBADF);

    ret = read(fd, buf, BUFSIZE);
    uassert(ret == 0);

    ret = write(fd, buf, BUFSIZE);
    uassert(ret == BUFSIZE);

    ret = close(fd);
    uassert(ret == STATUS_OK);

    cprintf("error code test successed!\n");
    return 0;
}
/* Test unlink and append.*/
int filetest5(int argc, char **argv)
{
    int fd, ret, i;
    char buf[BUFSIZE] = {0};

    for (i = 0; i < BUFSIZE; i++)
    {
        buf[i] = ('0'+ i )& 0xFF;
    }

    ret = unlink("test5");
    uassert(ret == -STATUS_ENOENT);

    fd = open("test5", O_WRONLY | O_CREAT, 0);
    uassert(fd >= STATUS_OK);

    ret = close(fd);
    uassert(ret == STATUS_OK);

    ret = unlink("test5");
    uassert(ret == STATUS_OK);

    fd = open("test5", O_RDWR, 0);
    uassert(fd == -STATUS_ENOENT); //file should be removed.

    fd = open("hello.txt", O_RDWR | O_APPEND, 0);
    uassert(fd >= STATUS_OK);

    ret = write(fd, buf, 10);
    uassert(ret == 10);

    ret = lseek(fd, 0, SEEK_SET); //seek to file begin
    uassert(ret == 0);

    ret = read(fd, buf, BUFSIZE);
    uassert(ret > STATUS_OK);

    cprintf("filetest5 read \"%s\"\n", buf);

    for (i = 0; i < ret; i++)
    {
        cprintf(" %c", buf[i]);
    }

    ret = close(fd);
    uassert(ret == STATUS_OK);

    return 0;
}

int fs_seek_test(int argc, char **argv)
{
    int i;
    off_t offset;
    int fd = -1;
    int ret;
    char buf[BUFSIZE] = {0};

    for (i = 0; i < BUFSIZE; i++)
    {
        buf[i] = i;
    }
    if ((fd = open("test2.txt", O_WRONLY | O_CREAT | O_TRUNC, 0)) >= 0)
    {
        ret = write(fd, buf, 10); // write test pattern
        if (ret == 10)
           cprintf("Write test pattern successed!\n");
        close(fd);
    }

    if ((fd = open("test2.txt", O_RDWR, 0)) >= 0)
    {
        offset = lseek(fd, 10, SEEK_END); //seek to file end + 10 bytes
        cprintf("File offset =%d\n");
        ret = write(fd, &(buf[10]), 10);
        offset = lseek(fd, 0, SEEK_SET); //seek to file begin

        for (i = 0; i < BUFSIZE; i++)
        {
            buf[i] = 0;
        }
        ret = read(fd, buf, BUFSIZE);

        if (ret >= 0)
        {
            cprintf("Read total %d bytes\n", ret);
            for (i = 0; i < ret; i++)
            {
                cprintf("%d ", buf[i]);
            }
            cprintf("\n");
        }
        close(fd);
    }
    else
        cprintf("Open failed!\n");
}
#define fsrw_fn                   "/test.dat"
#define fsrw_data_len             180              /* Less than 256 */
#define FS_TEST_TIMES       5000
int fs_speed_test(int argc, char **argv)
{
    int fd;
    int stop_flag = 0;
    int index,length;
    uint32_t round;
    uint32_t tick_start,tick_end,read_speed,write_speed;


    static uint8_t write_data[fsrw_data_len];
    static uint8_t read_data[fsrw_data_len];

    round = 0;

    while(round < 5)
    {

        /* creat file */
        fd = open(fsrw_fn, O_WRONLY | O_CREAT | O_TRUNC, 0);
        if (fd < 0)
        {
            cprintf("fsrw open file for write failed\n");
            return;
        }

        /* plan write data */
        for (index = 0; index < fsrw_data_len; index ++)
        {
            write_data[index] = index;
        }

        /* write N times */
        tick_start = get_ticks();
        for(index=0; index<FS_TEST_TIMES ; index++)
        {
            length = write(fd, write_data, fsrw_data_len);
            if (length != fsrw_data_len)
            {
                cprintf("fsrw write data failed\n");
                close(fd);
                return;
            }
        }
        tick_end = get_ticks();
        write_speed = fsrw_data_len*FS_TEST_TIMES *100/(tick_end-tick_start);

        /* close file */
        close(fd);

        /* open file read only */
        fd = open(fsrw_fn, O_RDONLY, 0);
        if (fd < 0)
        {
            cprintf("fsrw open file for read failed\n");
            return;
        }

        /* verify data */
        tick_start = get_ticks();
        for(index=0; index<FS_TEST_TIMES ; index++)
        {
            uint32_t i;

            length = read(fd, read_data, fsrw_data_len);
            if (length != fsrw_data_len)
            {
                cprintf("fsrw read file failed\r\n");
                close(fd);
                return;
            }
            for(i=0; i<fsrw_data_len; i++)
            {
                if( read_data[i] != write_data[i] )
                {
                    cprintf("fsrw data error!\r\n");
                    close(fd);
                    return;
                }
            }
        }
        tick_end = get_ticks();
        read_speed = fsrw_data_len*FS_TEST_TIMES *100/(tick_end-tick_start);

        cprintf("thread fsrw round %d ",round++);
        cprintf("rd:%dbyte/s,wr:%dbyte/s\r\n",read_speed,write_speed);

        /* close file */
        close(fd);
    }
}

void shell()
{
  char *buf;
  hist_head = 0;
  hist_tail = 0;
  hist_curr = 0;

  cprintf("Welcome to the OSDI course!\n");
  cprintf("Type 'help' for a list of commands.\n");

  while(1)
  {
    buf = readline("OSDI> ");
    if (buf != NULL)
    {
      strcpy(hist[hist_tail], buf);
      hist_tail = (hist_tail + 1) % SHELL_HIST_MAX;
      if (hist_head == hist_tail)
      {
        hist_head = (hist_head + 1) % SHELL_HIST_MAX;
        hist[hist_tail][0] = 0;
      }
      hist_curr = hist_tail;

      if (runcmd(buf) < 0)
        break;
    }
  }
}
