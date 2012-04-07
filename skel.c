#include <execinfo.h>
#include <mcheck.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void signal_handler (int sig)
{
  void *buffer[128];
  char **strings = NULL;
  size_t i = 0;
  size_t nptrs = 0;

  if (sig == SIGSEGV) {
    nptrs = backtrace (buffer, 128);
    printf ("backtrace() returned %d addresses\n", nptrs);

    strings = backtrace_symbols (buffer, nptrs);
    if (strings == NULL) {
      fprintf (stderr, "backtrace_symbols failed\n");
    } else {
      for (i = 0; i < nptrs; i++)
        fprintf (stderr, "%s\n", strings[i]);
      free(strings);
    }

    signal (sig, SIG_DFL);
    kill (getpid(), sig);
  }
}

int main(void)
{ 
  int *a = NULL;

  mtrace();

  signal (SIGSEGV, signal_handler);

  a = malloc(sizeof(int));

  muntrace();
                            
  return 0;
}

