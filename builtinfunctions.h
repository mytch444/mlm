#define init_built_in_functions() init_functions(built_in_functions, BUILT_IN_FUNCTIONS_N)

atom *add_function(atom *a);
atom *sub_function(atom *a);
atom *mul_function(atom *a);
atom *div_function(atom *a);
atom *less_than_function(atom *atoms);
atom *greater_than_function(atom *atoms);

atom *is_int(atom *a);
atom *is_float(atom *a);
atom *is_char(atom *a);

atom *to_int(atom *a);
atom *to_float(atom *a);
atom *to_char(atom *a);  

atom *equal_function(atom *atoms);

atom *is_list(atom *a);
atom *is_nil(atom *a);
atom *list_function(atom *atoms);
atom *cons_function(atom *atoms);
atom *car_function(atom *atoms);
atom *cdr_function(atom *atoms);

atom *cond_function(atom *atoms);

atom *lambda_function(atom *atoms);
atom *define_function(atom *atoms);

atom *exit_function(atom *atoms);
atom *progn_function(atom *atoms);

atom *eval_function(atom *atoms);

atom *do_lisp_function(atom *fa, atom *atoms);
atom *swap_in_args(atom *func, atom *args, atom *atoms);
atom *update_symbols(atom *atoms);

void init_functions(struct built_in_function functions[], int fn);

#define BUILT_IN_FUNCTIONS_N 25
built_in_function built_in_functions[BUILT_IN_FUNCTIONS_N] = {
  // name, function, accept_dirty, flat, argc.
  {"+", add_function, 0, 1, 3},
  {"-", sub_function, 0, 1, 3},
  {"*", mul_function, 0, 1, 3},
  {"/", div_function, 0, 1, 3},
  {"=", equal_function, 0, 1, 3},
  {"<", less_than_function, 0, 1, 3},
  {">", greater_than_function, 0, 1, 3},

  {"int?", is_int, 0, 1, 2},
  {"float?", is_float, 0, 1, 2},
  {"char?", is_char, 0, 1, 2},

  {"to-int", to_int, 0, 1, 2},
  {"to-float", to_float, 0, 1, 2},
  {"to-char", to_char, 0, 1, 2},
  
  {"list?", is_list, 0, 1, 2},
  {"nil?", is_nil, 0, 1, 2},

  {"list", list_function, 0, 1, -1},
  {"cons", cons_function, 0, 1, 3},
  {"car", car_function, 0, 1, 2},
  {"cdr", cdr_function, 0, 1, 2},

  {"cond", cond_function, 0, 0, -1},
  {"define", define_function, 1, 1, 3},
  {"\\", lambda_function, 1, 0, 3},

  {"exit", exit_function, 0, 0, 1},
  {"progn", progn_function, 0, 0, -1},

  {"eval", eval_function, 0, 1, 2},
};
