#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "atom.h"
#include "parse.h"
#include "evaluate.h"
#include "builtinfunctions.h"

#include "atom.c"
#include "parse.c"
#include "evaluate.c"
#include "builtinfunctions.c"

void repl(FILE *in);
void read_files(int argc, char **argv);
int main(int argc, char **argv);

void repl(FILE *in) {
  atom *parsed, *result;
  
  while (1) {
    printf("-> ");
    
    parsed = read_expression(in);
    if (!parsed)
      break;
    
    result = flatten(evaluate(parsed));
    printf("%s\n", atom_to_string(result));
  }
}

void read_files(int argc, char **argv) {
  FILE *f;
  int i;
  
  for (i = 1; i < argc; i++) {
    f = fopen(argv[i], "r");
    if (f == NULL) {
      continue;
    }
    
    repl(f);
    
    fclose(f);
  }
}
  
int main(int argc, char **argv) {
  init_functions(built_in_functions, BUILT_IN_FUNCTIONS_N);
  
  if (argc > 1) read_files(argc, argv);
  repl(stdin);
  return 0;
}
