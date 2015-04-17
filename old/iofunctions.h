// Functions for input and output.

atom *print_function(symbol *symbols, atom *atoms);
atom *open_function(symbol *symbols, atom *atoms);
atom *read_function(symbol *symbols, atom *atoms);

#define IO_FUNCTIONS_N 3
built_in_function io_functions[IO_FUNCTIONS_N] = {
  // name, function, accept_dirty, flat, argc.
  {"print", print_function, 1, 1, -2},
  {"open", open_function, 0, 1, 3},
  {"read", read_function, 0, 1, 4},
};
