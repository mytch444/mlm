// Data types.
#define INT 1
#define FLOAT 2
#define CHAR 3

#define isspace(X) (X == ' ' || X == '\n' || X == '\t')
#define isspecial(X) (X == '(' || X == ')' || X == '\"')
#define ischar(X) (!isspace(X) && !isspecial(X))
#define iscomment(X) (X == ';')
#define LINELENGTH 10000

// Define some atoms.
#define TRUE data_to_atom(int_to_data(1))
#define NIL nil_atom()

typedef struct function function;
typedef struct symbol symbol;
typedef struct atom atom;
typedef struct data data;

typedef struct built_in_function built_in_function;
struct built_in_function {
  char *name;
  atom *(*function)(symbol *symbols, atom *atoms);
  int accept_dirty, flat;
  int argc;
};

struct function {
  atom *args;
  int argc;
  atom *atoms;
  atom *function;
  atom *(*c_function)(symbol *symbols, atom *atoms);
  int accept_dirty, flat;
};

struct symbol {
  char *name;
  atom *atoms;
  symbol *next;
};

struct data {
  int type;
  int i;
  double f;
};

struct atom {
  atom *next;
  data *d;
  atom *s;
  function *f;
  char *sym;
};

typedef struct special_char special_char;
struct special_char {
  int c, s;
};

atom *nil_atom() {
  atom *n = malloc(sizeof(atom));
  n->next = NULL;
  n->d = NULL;
  n->s = NULL;
  n->f = NULL;
  n->sym = NULL;
  return n;
}

int end_expression_pos(char *string, int c);
char *string_cut(char *string, int start, int stop);

int string_is_int(char *string);
int string_is_float(char *string);
int string_is_char(char *string);
int string_is_string(char *string);

symbol *find_symbol(symbol *symbols, char *name);
void swap_symbols(symbol *symbols, atom *atoms);

void free_atom(atom *a);
void free_function(function *f);

atom *data_to_atom(data *d);

data *char_to_data(char c);
data *int_to_data(int i);
data *float_to_data(double f);

data *string_to_data(char *string);

int get_special_char(int c);
char *atom_string_to_string(atom *atoms);
atom *string_to_atom_string(char *string);
atom *constant_to_atom(char *name);

atom *parse(char *string);
atom *read_expression(FILE *f);

atom *evaluate(symbol *symbols, atom *raw);

atom *copy_atom(atom *a);
function *copy_function(function *f);
data *copy_data(data *a);
int isclean(atom *a);
atom *flatten(symbol *symbols, atom *o);
atom *do_sub(symbol *symbols, atom *a);
char *atom_to_string(atom *a);

void repl(symbol *symbols, FILE *in, int print);

#include "builtinfunctions.h"
