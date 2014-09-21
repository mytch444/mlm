#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "atom.h"
#include "parse.h"
#include "evaluate.h"
#include "builtinfunctions.h"

int main(int argc, char **argv);

void read_file(FILE *in);
void init_compiler_functions();

atom *includefunction();
atom *readfunction();

#define COMPILER_FUNCTIONS_N 2

built_in_function compiler_functions[COMPILER_FUNCTIONS_N] = {
  // name, function, accept_dirty, flat, argc.
  {"include", includefunction, 0, 1, 2},
  {"read", readfunction, 0, 1, 2},
};

#include "atom.c"
#include "parse.c"
#include "evaluate.c"
#include "builtinfunctions.c"

atom *includefunction() {
  
  return NIL;
}

atom *readfunction() {
  return NIL;
}

void read_file(FILE *f) {
  atom *parsed, *result;
  
  while (1) {
    parsed = read_expression(f);
    if (!parsed)
      break;

    printf("got '%s'\n", atom_to_string(parsed));
    printf("flattened '%s'\n", atom_to_string(flatten(parsed)));
    //    result = flatten(evaluate(parsed));
    //    printf("%s\n", atom_to_string(result));
  }
}
  
int main(int argc, char **argv) {
  char *filename;
  FILE *f;
  init_functions(built_in_functions, BUILT_IN_FUNCTIONS_N);
  init_functions(compiler_functions, COMPILER_FUNCTIONS_N);

  filename = argv[1];
  
  f = fopen(filename, "r");
  if (!f) {
    printf("Error: Could not open '%s'\n", filename);
    return 1;
  }
  
  read_file(f);

  return 0;
}
