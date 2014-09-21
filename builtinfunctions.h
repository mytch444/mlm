atom *addfunction(atom *a);
atom *subfunction(atom *a);
atom *mulfunction(atom *a);
atom *divfunction(atom *a);
atom *equalfunction(atom *atoms);
atom *lessthanfunction(atom *atoms);
atom *greaterthanfunction(atom *atoms);
atom *isint(atom *a);
atom *isfloat(atom *a);
atom *islist(atom *a);
atom *isnil(atom *a);
atom *listfunction(atom *atoms);
atom *consfunction(atom *atoms);
atom *carfunction(atom *atoms);
atom *cdrfunction(atom *atoms);
atom *condfunction(atom *atoms);
atom *printfunction(atom *atoms);
atom *lambdafunction(atom *atoms);
atom *definefunction(atom *atoms);
atom *do_lisp_function(atom *fa, atom *atoms);

atom *swap_in_args(atom *func, atom *args, atom *atoms);
atom *update_symbols(atom *atoms);

typedef struct built_in_function built_in_function;
struct built_in_function {
  char *name;
  atom *(*function)(atom *atoms);
  int accept_dirty, flat;
  int argc;
};

void init_functions(struct built_in_function functions[], int fn);

#define BUILT_IN_FUNCTIONS_N 19

built_in_function built_in_functions[BUILT_IN_FUNCTIONS_N] = {
  // name, function, accept_dirty, flat, argc.
  {"+", addfunction, 0, 1, 3},
  {"-", subfunction, 0, 1, 3},
  {"*", mulfunction, 0, 1, 3},
  {"/", divfunction, 0, 1, 3},
  {"=", equalfunction, 0, 1, 3},
  {"<", lessthanfunction, 0, 1, 3},
  {">", greaterthanfunction, 0, 1, 3},
  {"int?", isint, 0, 1, 2},
  {"float?", isfloat, 0, 1, 2},
  {"list?", islist, 0, 1, 2},
  {"nil?", isnil, 0, 1, 2},
  {"list", listfunction, 0, 1, -1},
  {"cons", consfunction, 0, 1, 3},
  {"car", carfunction, 0, 1, 2},
  {"cdr", cdrfunction, 0, 1, 2},
  {"cond", condfunction, 0, 0, -1},
  {"define", definefunction, 1, 1, 3},
  {"\\", lambdafunction, 1, 0, 3},
  {"print", printfunction, 1, 1, -2},
};

symbol *symbols;
