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
#define FUNCTION 4

#define TRUE data_to_atom(int_to_data(1))
#define NIL nil_atom()

#define INFINITE_ARGS -1

typedef struct function function;
typedef struct realfunction realfunction;
typedef struct built_in_function built_in_function;
typedef struct atom atom;
typedef struct data data;

struct built_in_function {
  char *name;
  atom *(*function)(atom *atoms);
  int accept_dirty, flat;
  int argc;
};

struct realfunction {
  char *name;
  atom *args;
  int argc;
  atom *function;
  atom *(*b_function)(atom *atoms);
  int accept_dirty, flat;
  realfunction *next;
};

struct function {
  char *name;
  atom *atoms;
  realfunction *function;
};

struct data {
  int type;
  int i;
  double f;
};

struct atom {
  atom *next;
  data *d;
  atom *a;
  function *f;
};

void read_files(char **argv, int argc);
void repl(FILE *in, int print);
void init_default_functions();

realfunction *find_function(char *name);
function *real_function_to_function(realfunction *f);

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

data *copy_data(data *a);
function *copy_function(function *f);
atom *copy_atom(atom *a);

int string_is_int(char *string);
int string_is_float(char *string);
int string_is_string(char *string);
int string_is_char(char *string);

atom *nil_atom();

realfunction *functions;

#include "builtinfunctions.c"

#define BUILT_IN_FUNCTIONS_N 15

built_in_function built_in_functions[BUILT_IN_FUNCTIONS_N] = {
  {"+", addfunction, 0, 1, 3},
  {"-", subfunction, 0, 1, 3},
  {"*", mulfunction, 0, 1, 3},
  {"/", divfunction, 0, 1, 3},
  {"=", equalfunction, 0, 1, INFINITE_ARGS},
  {"int?", isint, 0, 1, 2},
  {"float?", isfloat, 0, 1, 2},
  {"list?", islist, 0, 1, 2},
  {"nil?", isnil, 0, 1, 2},
  {"list", listfunction, 0, 1, INFINITE_ARGS},
  {"cons", consfunction, 0, 1, 3},
  {"car", carfunction, 0, 1, 2},
  {"cdr", cdrfunction, 0, 1, 2},
  {"cond", condfunction, 0, 0, INFINITE_ARGS},
  {"defun", defunfunction, 1, 0, 4},
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
  
  return d;
}

data *int_to_data(int i) {
  data *d = malloc(sizeof(data));
  
  d->type = INT;
  d->i = i;
  
  return d;
}

data *float_to_data(double f) {
  data *d = malloc(sizeof(data));
  
  d->type = FLOAT;
  d->f = f;
  
  return d;
}

atom *nil_atom() {
  atom *a = malloc(sizeof(atom));
  a->d = NULL;
  a->a = NULL;
  a->f = NULL;
  a->next = NULL;
  return a;
}

atom *do_sub(atom *a) {
  if (!a || !a->a) return a;
  atom *r = malloc(sizeof(atom));
  atom *s = evaluate(a->a);
  
  if (s->next) {
    r->d = NULL;
    r->f = NULL;
    r->a = s;
  } else {
    r->d = s->d;
    r->a = s->a;
    r->f = s->f;
  }

  r->next = a->next;
  return r;
}

atom *flatten(atom *o) {
  atom *a;

  if (o->a)
    o = do_sub(o);

  for (a = o; a && a->next; a = a->next)
    if (a->next->a)
      a->next = do_sub(a->next);

  return o;
}

int clean(atom *a) {
  for (; a; a = a->next)
    if (a->f && !a->f->function)
      return 0;
    else if (a->a)
      if (!clean(a->a))
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
  } else if (a->a) {
    sprintf(result, "(%s)", atom_to_string(a->a));
  } else if (a->f) {
    sprintf(result, "[%s %s]", a->f->name, atom_to_string(a->f->atoms));
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
  a->a = NULL;
  a->f = NULL;
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

  r->name = f->name;
  r->atoms = copy_atom(f->atoms);
  r->function = f->function;

  return r;
}

atom *copy_atom(atom *a) {
  if (!a) return NULL;
  atom *b = malloc(sizeof(atom));
  b->d = copy_data(a->d);
  b->a = copy_atom(a->a);
  b->f = copy_function(a->f);
  b->next = copy_atom(a->next);
  return b;
}

realfunction *find_function(char *name) {
  realfunction *f;
  for (f = functions; f; f = f->next)
    if (strcmp(f->name, name) == 0)
      return f;
  
  return NULL;
}

function *real_function_to_function(realfunction *f) {
  if (!f)
    return NULL;

  function *r = malloc(sizeof(function));
  r->name = f->name;
  r->function = f;
  r->atoms = NULL;
  return r;
}

atom *evaluate(atom *atoms) {
  int argc, i;
  atom *a, *r, *args, *rest;

  printf("evaluating '%s'\n", atom_to_string(atoms));
  
  if (!atoms) return NIL;
  
  if (atoms->a)
    atoms = do_sub(atoms);

  if (atoms && atoms->f) {
    if (!atoms->f->function) {
      return atoms;
    }

    args = atoms->f->atoms;
    if (args)
      args->next = atoms->next;
    else
      args = atoms->next;
    
    rest = NULL;
    
    argc = atoms->f->function->argc;
    for (i = 0, a = args; a; a = a->next, i++);
    
    if (argc != INFINITE_ARGS && i > argc) {
      atom *prev = NULL;
      for (i = 0, a = args; a; prev = a, a = a->next, i++) {
	if (i >= argc) {
	  rest = a;
	  if (prev)
	    prev->next = NULL;
	  break;
	}
      }

    } else if (i < argc) {
      atoms->f->atoms = args;
      atoms->next = NULL;

      return atoms;
    }
    
    if (atoms->f->function->b_function) {
      if (atoms->f->function->accept_dirty || clean(args)) {
	if (atoms->f->function->flat) {
	  r = atoms->f->function->b_function(flatten(args));
	} else {
	  r = atoms->f->function->b_function(args);
	}
      } else {
	r = atoms;
      }
    } else {
      r = do_lisp_function(atoms, args);
    }

    for (a = r; a && a->next; a = a->next);
    if (a)
      a->next = rest;
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
      break;
    
    if (string[i] == '\"') {
      start = i + 1;
      for (i++; string[i] && string[i] != '\"'; i++);
      
      char *chars = string_cut(string, start, i);
      atoms->next = malloc(sizeof(atom));
      atoms = atoms->next;
      atoms->d = NULL;
      atoms->a = malloc(sizeof(atom));
      atoms->f = NULL;
      atoms->next = NULL;
      
      t = atoms->a;
      for (j = 0; chars[j]; j++) {
	t->d = char_to_data(chars[j]);
	t->a = NULL;
	t->next = malloc(sizeof(atom));
	t = t->next;
	t->d = NULL;
	t->f = NULL;
	t->a = NULL;
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
      atoms->a = NULL;
      atoms->f = NULL;
      atoms->next = NULL;
      
      if (d) {
	atoms->d = d;
      } else {
	function *f = real_function_to_function(find_function(name));
	if (f) {
	  atoms->f = f;
	} else {
	  atoms->f = malloc(sizeof(function));
	  atoms->f->name = name;
	  atoms->f->atoms = NULL;
	  atoms->f->function = NULL;
	}
      }
      
      num++;
    } else if (string[i] == '(') {
      start = i + 1;
      end = closing_bracket_pos(string, start - 1);
      atom *sub = parse(string_cut(string, start, end));
      
      if (sub) {
	atoms->next = malloc(sizeof(atom));
	atoms = atoms->next;
	atoms->next = NULL;
	atoms->d = NULL;
	atoms->f = NULL;
	atoms->a = sub;
	
	atom *a;
	for (a = atoms->a; a && a->next; a = a->next);
	a->next = NIL;
      } else {
	atoms->next = NIL;
	atoms = atoms->next;
      }
      
      num++;
      i = end + 1;
    } else {
      printf("I have no idea what to do with this char '%c'\n", string[i]);
      i++;
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
  int c, d, open, close, inquote;
  
  while (1) {
    if (print) printf("-> ");
    
    open = close = inquote = 0;
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
	if (line[c] == '\'' || line[c] == '\"') inquote = !inquote;
	else if (line[c] == '(') open++;
	else if (line[c] == ')') close++;
      
    } while (open != close || inquote);
    
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
  realfunction *f;
 
  functions = malloc(sizeof(realfunction));
  for (i = 0, f = functions; i < BUILT_IN_FUNCTIONS_N; i++) {
    f->name = built_in_functions[i].name;
    f->args = NULL;
    f->argc = built_in_functions[i].argc;
    f->function = NULL;
    f->b_function = built_in_functions[i].function;
    f->accept_dirty = built_in_functions[i].accept_dirty;
    f->flat = built_in_functions[i].flat;
    f->next = NULL;

    if (i < BUILT_IN_FUNCTIONS_N - 1) {
      f->next = malloc(sizeof(realfunction));
      f = f->next;
    }
  }
}
  
int main(int argc, char **argv) {
  init_built_in_functions();
  
  if (argc > 1) read_files(argv, argc);
  repl(stdin, 1);
  return 0;
}
