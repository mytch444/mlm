#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "mlm.h"
#include "iofunctions.h"

#include "mlm.c"
#include "iofunctions.c"

void read_files(symbol *symbols, int argc, char **argv) {
  FILE *f;
  int i;
  
  for (i = 1; i < argc; i++) {
    printf("Loading %s...", argv[i]);
    f = fopen(argv[i], "r");
    if (f == NULL) {
      continue;
    }

    exit_repl = 0;
    repl(symbols, f, 0);

    fclose(f);
    printf("Success\n");
  }
}
  
int main(int argc, char **argv) {
  symbol *symbols;
    
  symbols = init_built_in_functions(symbols);
  symbols = init_functions(symbols, io_functions, IO_FUNCTIONS_N);

  char **defaults = malloc(sizeof(char*) * 3);
  defaults[0] = NULL;
  defaults[1] = "lisp/funcs.l";
  defaults[2] = NULL;
  
  read_files(symbols, 2, defaults);
  
  if (argc > 1) read_files(symbols, argc, argv);
  repl(symbols, stdin, 1);
  return 0;
}
