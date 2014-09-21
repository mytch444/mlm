// Data types.
#define INT 1
#define FLOAT 2
#define CHAR 3

// Define some atoms.
#define TRUE data_to_atom(int_to_data(1))
#define NIL nil_atom()

typedef struct function function;
typedef struct symbol symbol;
typedef struct atom atom;
typedef struct data data;
