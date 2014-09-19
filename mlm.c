#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LINELENGTH 10000

#define isspace(X) (X == ' ' || X == '\n' || X == '\t')
#define isspecial(X) (X == '(' || X == ')' || X == '\"')
#define ischar(X) (!isspace(X) && !isspecial(X))
#define iscomment(X) (X == ';')

#define INT 1
#define FLOAT 2
#define CHAR 3

#define TRUE data_to_atom(int_to_data(1))
#define NIL nil_atom()

typedef struct function function;
typedef struct symbol symbol;
typedef struct built_in_function built_in_function;
typedef struct atom atom;
typedef struct data data;

struct built_in_function {
  char *name;
  atom *(*function)(atom *atoms);
  int accept_dirty, flat;
  int argc;
};

struct function {
  atom *args;
  int argc;
  atom *atoms;
  atom *function;
  atom *(*b_function)(atom *atoms);
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
  symbol *sym;
};

void read_files(char **argv, int argc);
void repl(FILE *in, int print);
void init_default_functions();

atom *swap_symbols(atom *atoms);
symbol *find_symbol(char *name);

atom *parse(char *string);
atom *evaluate(atom *atoms);

char *string_cut(char *string, int start, int stop);
int closing_bracket_pos(char *string, int open);

data *string_to_data(char *string);
data *char_to_data(char c);
data *int_to_data(int i);
data *float_to_data(double f);

atom *data_to_atom(data *d);

char *atom_to_string(atom *a);

atom *do_sub(atom *a);
atom *flatten(atom *o);
int isclean(atom *a);

data *copy_data(data *a);
function *copy_function(function *f);
atom *copy_atom(atom *a);

int string_is_int(char *string);
int string_is_float(char *string);
int string_is_string(char *string);
int string_is_char(char *string);

atom *nil_atom();

symbol *symbols;

#include "builtinfunctions.c"

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

int closing_bracket_pos(char *string, int open) {
  int i;
  int brackets = 0;
  for (i = open; string[i] != '\0'; i++) {
    if (string[i] == '(') brackets++;
    if (string[i] == ')') {
      if (brackets == 1) return i;
      else brackets--;
    }
  }
  
  return -1;
}

char *string_cut(char *string, int start, int stop) {
  int i;
  char *new = malloc(sizeof(char) * (stop - start + 1));
  for (i = 0; i < stop - start; i++) new[i] = string[start + i];
  new[i] = '\0';
  return new;
}

int string_is_int(char *string) {
  int i = 0;
  if (string[i] == '-' && string[i + 1])
    i++;
  for (; string[i]; i++)
    if (!(string[i] >= '0' && string[i] <= '9'))
      return 0;
  return 1;
}

int string_is_float(char *string) {
  int i = 0;
  int point = 0;
  if (string[i] == '-' && string[i + 1])
    i++;
  for (; string[i]; i++) {
    if (string[i] == '.' && !point) {
      point = 1;
      continue;
    }
    if (!(string[i] >= '0' && string[i] <= '9'))
      return 0;
  }
  return 1;
}

int string_is_char(char *string) {
  int i;
  if (string[0] == '\'' && string[2] == '\'')
    return 1;
  return 0;
}

int string_is_string(char *string) {
  int i;
  for (i = 0; string[i] && string[i + 1]; i++);
  if (string[0] == '\"' && string[i] == '\"')
    return 1;
  return 0;
}

data *string_to_data(char *string) {
  if (string_is_int(string)) {
    return int_to_data(atoi(string));
    
  } else if (string_is_float(string)) {
    return float_to_data(atof(string));
    
  } else if (string_is_char(string)) {
    return char_to_data(string[1]);
    
  } else return NULL;
}

data *char_to_data(char c) {
  data *d = malloc(sizeof(data));
  
  d->type = CHAR;
  d->i = c;
  d->f = 0;
  
  return d;
}

data *int_to_data(int i) {
  data *d = malloc(sizeof(data));
  
  d->type = INT;
  d->i = i;
  d->f = 0;
  
  return d;
}

data *float_to_data(double f) {
  data *d = malloc(sizeof(data));
  
  d->type = FLOAT;
  d->f = f;
  d->i = 0;
  
  return d;
}

atom *nil_atom() {
  atom *a = malloc(sizeof(atom));
  a->d = NULL;
  a->s = NULL;
  a->f = NULL;
  a->sym = NULL;
  a->next = NULL;
  return a;
}

atom *do_sub(atom *a) {
  if (!a || !a->s) return a;
  atom *r = malloc(sizeof(atom));
  atom *s = evaluate(a->s);
  
  if (s->next) {
    r->sym = NULL;
    r->d = NULL;
    r->f = NULL;
    r->s = s;
  } else {
    r->sym = s->sym;
    r->d = s->d;
    r->s = s->s;
    r->f = s->f;
  }

  r->next = a->next;
  return r;
}

atom *flatten(atom *o) {
  atom *a;

  if (o->s)
    o = do_sub(o);

  for (a = o; a && a->next; a = a->next)
    if (a->next->s) {
      a->next = do_sub(a->next);
    } else if (a->next->sym) {
      printf("WHY THE FUCK IS THERE A SYMBOL HERE!!!\n\n");
    }

  return o;
}

int isclean(atom *a) {
  for (; a; a = a->next)
    if (a->sym && !a->sym->atoms) {
      return 0;
    } else if (a->s)
      if (!isclean(a->s))
	return 0;
  return 1;
}

char *atom_to_string(atom *a) {
  char *result = malloc(sizeof(char) * 1000);
  result[0] = '\0';
  
  if (!a)
    return "";
  if (a->d) {
    if (a->d->type == INT) {
      sprintf(result, "%i", a->d->i);
    } else if (a->d->type == FLOAT) {
      sprintf(result, "%g", a->d->f);
    } else if (a->d->type == CHAR) {
      sprintf(result, "'%c'", a->d->i);
    } else {
      sprintf(result, "ERROR: What the fuck is this!");
    }
  } else if (a->s) {
    atom *s;
    int i;
    for (i = 0, s = a->s; s && s->next; s = s->next, i++) {
      if (!s->d || s->d->type != CHAR) {
	result[0] = '\0';
	break;
      }

      result[i] = s->d->i;
    }

    if (!result[0])
      sprintf(result, "(%s)", atom_to_string(a->s));
    
  } else if (a->f) {
    sprintf(result, "f");
  } else if (a->sym) {
    sprintf(result, "`%s`", a->sym->name);
  } else {
    sprintf(result, "()");
  }
  
  if (a->next) {
    char *next = atom_to_string(a->next);
    char *copy = malloc(sizeof(char) * 1000);
    strcpy(copy, result);
    sprintf(result, "%s %s", copy, next);
  }
  
  return result;
}

atom *data_to_atom(data *d) {
  atom *a = malloc(sizeof(atom));
  a->d = d;
  a->s = NULL;
  a->f = NULL;
  a->sym = NULL;
  a->next = NULL;
  return a;
}

data *copy_data(data *a) {
  if (!a) return NULL;
  data *b = malloc(sizeof(data));
  b->type = a->type;
  b->i = a->i;
  b->f = a->f;
  return b;
}

function *copy_function(function *f) {
  if (!f) return NULL;
  function *r;
  r = malloc(sizeof(function));

  r->args = copy_atom(f->args);
  r->argc = f->argc;
  r->atoms = copy_atom(f->atoms);
  r->function = copy_atom(f->function);
  r->b_function = f->b_function;
  r->accept_dirty = f->accept_dirty;
  r->flat = f->flat;

  return r;
}

atom *copy_atom(atom *a) {
  if (!a) return NULL;
  atom *b = malloc(sizeof(atom));
  b->d = copy_data(a->d);
  b->s = copy_atom(a->s);
  b->f = copy_function(a->f);
  b->sym = a->sym;
  b->next = copy_atom(a->next);
  return b;
}

symbol *find_symbol(char *name) {
  symbol *s;
  for (s = symbols; s; s = s->next)
    if (strcmp(s->name, name) == 0)
      return s;

  return NULL;
}

atom *swap_symbols(atom *atoms) {
  atom *a, *s;
  for (a = atoms; a; a = a->next) {
    if (a->sym && a->sym->atoms) {
      s = a->sym->atoms;
      a->d = copy_data(s->d);
      a->s = copy_atom(s->s);
      a->f = copy_function(s->f);
      a->sym = NULL;
    } else if (a->s) {
      a->s = swap_symbols(a->s);
    }
  }

  return atoms;
}

atom *evaluate(atom *raw) {
  int argc, i;
  atom *atoms, *a, *r, *args;

  atoms = swap_symbols(raw);
  
  if (!atoms) return NIL;
  
  if (atoms->s)
    atoms = do_sub(atoms);

  if (atoms->f) {
    if (!atoms->f->function && !atoms->f->b_function)
      return atoms;

    args = atoms->f->atoms;
    if (args)
      args->next = atoms->next;
    else
      args = atoms->next;
    
    argc = atoms->f->argc;
    for (i = 0, a = args; a; a = a->next, i++);
    
    if (argc > 0 && i > argc) {
      printf("too many arguments\n");
      atom *prev = NULL;
      for (i = 0, a = args; a; prev = a, a = a->next, i++) {
	if (i >= argc) {
	  prev->next = NULL;
	  break;
	}
      }

    } else if (i < argc || (argc < 0 && i < -argc)) {
      printf("Not enough arguments\n");
      atoms->f->atoms = args;
      atoms->next = NULL;

      return atoms;
    }

    if (atoms->f->flat) {
      a = flatten(args);
    } else
      a = args;
      
    if (atoms->f->b_function) {
      if (atoms->f->accept_dirty || isclean(a)) {
	r = atoms->f->b_function(a);
      } else {
	r = a;
      }
    } else {
      r = do_lisp_function(atoms, a);
    }

    return r;
  } else
    return atoms;
}

atom *parse(char *string) {
  int num, i, start, end, j;
  atom *handle, *atoms, *t;
  
  handle = malloc(sizeof(atom));
  handle->next = NULL;
  atoms = handle;
  num = 0;
  i = 0;
  while (string[i]) {
    for (; string[i] && isspace(string[i]); i++);
    
    if (!string[i])
      break;
    
    if (iscomment(string[i]))
      for (; string[i] && string[i] != '\n'; i++);
    
    if (string[i] == '\"') {
      start = i + 1;
      for (i++; string[i] && string[i] != '\"'; i++);
      i++;
      
      char *chars = string_cut(string, start, i - 1);
      atoms->next = malloc(sizeof(atom));
      atoms = atoms->next;
      atoms->d = NULL;
      atoms->s = malloc(sizeof(atom));
      atoms->f = NULL;
      atoms->sym = NULL;
      atoms->next = NULL;

      t = atoms->s;
      for (j = 0; chars[j]; j++) {
	t->d = char_to_data(chars[j]);
	t->s = NULL;
	t->f = NULL;
	t->sym = NULL;
	t->next = malloc(sizeof(atom));
	t = t->next;
	t->d = NULL;
	t->f = NULL;
	t->s = NULL;
	t->sym = NULL;
	t->next = NULL;
      }

      num++;
    } else if (ischar(string[i])) {
      start = i;
      for (; string[i] && ischar(string[i]); i++);
      char *name = string_cut(string, start, i);
      
      data *d = string_to_data(name);
      
      atoms->next = malloc(sizeof(atom));
      atoms = atoms->next;
      atoms->d = NULL;
      atoms->s = NULL;
      atoms->f = NULL;
      atoms->sym = NULL;
      atoms->next = NULL;

      if (d) {
	atoms->d = d;
      } else {
	symbol *s = find_symbol(name);
	if (s) {
	  atoms->sym = s;
	} else {
	  atoms->sym = malloc(sizeof(symbol));
	  atoms->sym->name = name;
	  atoms->sym->atoms = NULL;
	  atoms->sym->next = NULL;
	}
      }
      
      num++;
    } else if (string[i] == '(') {
      start = i + 1;
      end = closing_bracket_pos(string, start - 1);
      if (start > end) {
	printf("Error parsing\n");
	return NULL;
      }
      
      atom *sub = parse(string_cut(string, start, end));
      
      if (sub) {
	atoms->next = malloc(sizeof(atom));
	atoms = atoms->next;
	atoms->next = NULL;
	atoms->d = NULL;
	atoms->f = NULL;
	atoms->sym = NULL;
	atoms->s = sub;
	
	atom *a;
	for (a = atoms->s; a && a->next; a = a->next);
	a->next = NIL;
      } else {
	atoms->next = NIL;
	atoms = atoms->next;
      }
      
      num++;
      i = end + 1;
    } else {
      printf("I have no idea what to do with this char '%c'\n", string[i]);
      return NULL;
    }
  }
  
  return handle->next;
}

void read_files(char **argv, int argc) {
  FILE *f;
  int i;
  
  for (i = 1; i < argc; i++) {
    f = fopen(argv[i], "r");
    if (f == NULL) {
      continue;
    }
    
    repl(f, 0);
    
    fclose(f);
  }
}

void repl(FILE *in, int print) {
  char line[500];
  char string[5000];
  atom *result;
  atom *parsed;
  int c, d, open, close, inquote, inchar;
  
  while (1) {
    if (print) printf("-> ");
    
    open = close = inquote = inchar = 0;
    string[0] = '\0';
    line[0] = '\0';
    
    do {
      if (!fgets(line, sizeof(char) * 500, in))
	return;
      
      for (c = 0; string[c]; c++);
      for (d = 0; line[d]; d++)
	string[c + d] = line[d];
      string[c + d] = '\0';
      
      for (c = 0; line[c]; c++)
	if (!inquote && line[c] == '\'') inchar = !inchar;
	else if(line[c] == '\"') inquote = !inquote;
	else if (line[c] == '(' && !inquote) open++;
	else if (line[c] == ')' && !inquote) close++;
      
    } while (open != close || inquote || inchar);
    
    for (c = 0; string[c] && string[c + 1]; c++);
    if (string[c] == '\n')
      string[c] = '\0';
    
    parsed = parse(string);
    result = flatten(evaluate(parsed));
    if (print) printf("%s\n", atom_to_string(result));
  }
}

void init_built_in_functions() {
  int i;
  function *f;
  symbol *s;
  atom *a;
 
  symbols = malloc(sizeof(symbol));
  for (i = 0, s = symbols; i < BUILT_IN_FUNCTIONS_N; i++) {

    f = malloc(sizeof(function));
    f->args = NULL;
    f->argc = built_in_functions[i].argc;
    f->atoms = NULL;
    f->function = NULL;
    f->b_function = built_in_functions[i].function;
    f->accept_dirty = built_in_functions[i].accept_dirty;
    f->flat = built_in_functions[i].flat;

    a = malloc(sizeof(atom));
    a->f = f;
    a->s = NULL;
    a->d = NULL;
    a->sym = NULL;
    a->next = NULL;

    s->name = built_in_functions[i].name;
    s->atoms = a;
    s->next = NULL;

    if (i + 1 < BUILT_IN_FUNCTIONS_N) {
      s->next = malloc(sizeof(symbol));
      s = s->next;
    }
  }
}
  
int main(int argc, char **argv) {
  init_built_in_functions();
  
  if (argc > 1) read_files(argv, argc);
  repl(stdin, 1);
  return 0;
}
