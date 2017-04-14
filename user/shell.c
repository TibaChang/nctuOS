#include <inc/syscall.h>
#include <inc/stdio.h>
#include <inc/string.h>
#include <inc/shell.h>

char hist[SHELL_HIST_MAX][BUF_LEN];

int hist_head;
int hist_tail;
int hist_curr;

/*  Prototypes  */
int mon_help(int argc, char **argv);
int mem_stat(int argc, char **argv);
int print_tick(int argc, char **argv);
int chgcolor(int argc, char **argv);
int forktest(int argc, char **argv);

struct Command commands[] = {
  { "help", "Display this list of commands", mon_help },
  { "mem_stat", "Show current usage of physical memory", mem_stat },
  { "print_tick", "Display system tick", print_tick },
  { "chgcolor", "Change screen text color", chgcolor },
  { "forktest", "Test functionality of fork()", forktest }
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
	int i;

	pid = getpid();
	for (i = 0; i < 10; i++)
	{
		cprintf("Im %d, now=%d\n", pid, i);
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
  }
  /* task recycle */
  kill_self();
  return 0;
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
