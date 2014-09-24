
atom *print_function(atom *atoms);
atom *read_function(atom *atoms);

#define IO_FUNCTIONS_N 2

built_in_function io_functions[IO_FUNCTIONS_N] = {
  {"print", print_function, 1, 1, -2},
  {"read", read_function, 0, 0, 1},
};
