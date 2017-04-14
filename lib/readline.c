#include <inc/stdio.h>
#include <inc/kbd.h>
#include <inc/shell.h>
#include <inc/error.h>
#include <inc/syscall.h>
#include <inc/string.h>

#define BUFLEN 1024
static char buf[BUFLEN];
static int tab_idx;
static int is_tab;
static int is_found;

extern struct Command commands[];
extern int NCOMMANDS;

extern char hist[SHELL_HIST_MAX][BUF_LEN];

extern int hist_head;
extern int hist_tail;
extern int hist_curr;

char *readline(const char *prompt)
{
  int i, c;

  if (prompt != NULL)
    cprintf("%s", prompt);

  i = 0;
  while (1) {
    c = getchar();

    // Fill the found command into current buffer
    if (is_tab && is_found && c != '\t') {
      strcpy(buf, commands[tab_idx].name);
      i = strlen(buf);
      tab_idx = 0;
      is_tab = 0;
      is_found = 0;
    }

    if (c < 0) {
      cprintf("read error: %e\n", c);
      return NULL;
    } else if ((c == '\b' || c == '\x7f') && i > 0) {
      cprintf("\b");
      i--;
    } else if (c >= ' ' && c <= 0x7E && i < BUFLEN-1) {
      cprintf("%c",c);
      buf[i++] = c;
    } else if (c == '\n' || c == '\r') {
      cprintf("\n");
      buf[i] = 0;
      return buf;
    } else if (c == 0xc) { // ctrl-L or ctrl-l
      cls();
      cprintf("%s", prompt);
    } else if (c == '\t') {
      // Start from previous match
      int curr_idx = (is_found) ? tab_idx+1 : 0;

      if (is_found) {
        // Clear the output of previous match
        int tmp;
        for (tmp = 0 ; tmp < strlen(commands[tab_idx].name)-i ; tmp ++)
          cprintf("\b");
      }

      for (curr_idx ; curr_idx < NCOMMANDS ; curr_idx ++) {
        if (strncmp(commands[curr_idx].name, buf, i) == 0) {
          // Show the found command, instead of changing the current buffer
          cprintf("%s", commands[curr_idx].name+i);
          is_tab = 1;
          is_found = 1;
          tab_idx = curr_idx;
          break;
        }
      }
      if (curr_idx == NCOMMANDS) {
          is_found = 0;
      }

    } else if (c >= 0x80) {
      switch (c)
      {
        case KEY_UP:
          if (hist_curr != hist_head)
            hist_curr = (hist_curr == 0) ? SHELL_HIST_MAX-1 : hist_curr-1;

          while (i --)
            cprintf("\b");
          for (i = 0 ; hist[hist_curr][i] ; i ++) {
            buf[i] = hist[hist_curr][i];
            cprintf("%c",buf[i]);
          }
          break;
        case KEY_DN:
          if (hist_curr != hist_tail)
            hist_curr = (hist_curr == SHELL_HIST_MAX-1) ? 0 : hist_curr+1;

          while (i --)
            cprintf("\b");
          for (i = 0 ; hist[hist_curr][i] ; i ++) {
            buf[i] = hist[hist_curr][i];
            cprintf("%c",buf[i]);
          }
          break;
      }
    }
  }
}

