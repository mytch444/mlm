#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "atom.h"
#include "parse.h"
#include "evaluate.h"
#include "builtinfunctions.h"
#include "iofunctions.h"
#include "repl.h"

#include "atom.c"
#include "parse.c"
#include "evaluate.c"
#include "builtinfunctions.c"
#include "iofunctions.c"
#include "repl.c"

void read_files(int argc, char **argv);
int main(int argc, char **argv);

void read_files(int argc, char **argv) {
  FILE *f;
  int i;
  
  for (i = 1; i < argc; i++) {
    f = fopen(argv[i], "r");
    if (f == NULL) {
      continue;
    }

    exit_repl = 0;
    repl(f, 0);
    
    fclose(f);
  }
}
  
int main(int argc, char **argv) {
  init_functions(built_in_functions, BUILT_IN_FUNCTIONS_N);
  init_functions(io_functions, IO_FUNCTIONS_N);

  if (argc > 1) read_files(argc, argv);
  repl(stdin, 1);
  return 0;
}
