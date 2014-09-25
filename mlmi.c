#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mlm.h"
#include "iofunctions.h"

#include "mlm.c"
#include "iofunctions.c"

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
  init_built_in_functions();
  init_functions(io_functions, IO_FUNCTIONS_N);

  if (argc > 1) read_files(argc, argv);
  repl(stdin, 1);
  return 0;
}
