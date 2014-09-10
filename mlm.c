#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LINELENGTH 10000

#define isspace(X) (X == ' ' || X == '\n' || X == '\t')
#define isspecial(X) (X == '(' || X == ')')
#define ischar(X) (!isspace(X) && !isspecial(X))
#define iscomment(X) (X == ';')

#define INT 1
#define FLOAT 2
#define CHAR 3
#define FUNCTION 4

#define TRUE data_to_atom(int_to_data(1))
#define NIL nil_atom()

typedef struct function function;
typedef struct lispfunction lispfunction;
typedef struct atom atom;
typedef struct data data;

struct function {
  char *name;
  atom *(*function)(atom *handle, atom *atoms);
  function *next;
};

struct lispfunction {
  char *name;
  atom *args;
  int argc;
  atom *function;
  lispfunction *next;
};

struct data {
  int type;
  int i;
  double f;
  char *string;
};

struct atom {
  data *d;
  atom *a;
  atom *next;
};

void read_files(char **argv, int argc);
void repl(FILE *in, int print);
void init_default_functions();
  
function *get_function(atom *a);

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

data *copy_data(data *a);
atom *copy_atom(atom *a);

int string_is_int(char *string);
int string_is_float(char *string);
int string_is_string(char *string);
int string_is_char(char *string);

atom *nil_atom();

function *functions;
lispfunction *lispfunctions;

#include "builtinfunctions.c"

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
  } else {
    data *d = malloc(sizeof(data));
    
    d->type = FUNCTION;
    d->string = string;
    
    return d;
  }
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
  a->next = NULL;
  return a;
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
      sprintf(result, "%s", a->d->string);
    }
  } else if (a->a) {
    sprintf(result, "(%s)", atom_to_string(a->a));
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
  a->next = NULL;
  return a;
}

data *copy_data(data *a) {
  if (!a) return NULL;
  data *b = malloc(sizeof(data));
  b->type = a->type;
  b->i = a->i;
  b->f = a->f;
  b->string = a->string;
  return b;
}

atom *copy_atom(atom *a) {
  if (!a) return NULL;
  atom *b = malloc(sizeof(atom));
  b->d = copy_data(a->d);
  b->a = copy_atom(a->a);
  b->next = copy_atom(a->next);
  return b;
}

function *get_function(atom *a) {
  function *f;
  if (!a || !a->d) return NULL;
  for (f = functions; f; f = f->next)
    if (strcmp(f->name, a->d->string) == 0)
      return f;
  return NULL;
}

atom *evaluate(atom *atoms) {
  int i;
  atom *a;

  if (!atoms) return NIL;

  if (atoms->a) {
    atom *s = evaluate(atoms->a);
    if (!s) return NIL;
    if (s->next) {
      atoms->d = NULL;
      atoms->a = s;
    } else {
      atoms->d = s->d;
      atoms->a = s->a;
    }
  }
  
  if (atoms->d) {
    if (atoms->d->type == FUNCTION) {
      function *f = get_function(atoms);
      if (f)
	return f->function(atoms, atoms->next);
      else
	return atoms;
    } else {
      return atoms;
    }
  } else if (atoms->a) {
    return atoms;
  } else
    return NIL;
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
    
    if (ischar(string[i])) {
      start = i;
      for (; string[i] && ischar(string[i]); i++);

      char *sub = string_cut(string, start, i);
      
      atoms->next = malloc(sizeof(atom));
      atoms = atoms->next;
      if (string_is_string(sub)) {
	char *chars = string_cut(string, start + 1, i - 1);
	atoms->d = NULL;
	atoms->a = malloc(sizeof(atom));
	t = atoms->a;
	for (j = 0; chars[j]; j++) {
	  t->d = char_to_data(chars[j]);
	  t->a = NULL;
	  t->next = malloc(sizeof(atom));
	  t = t->next;
	}
      } else {
	atoms->d = string_to_data(sub);
	atoms->a = NULL;
      }
      atoms->next = NULL;
	     
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
	atoms->a = sub;

	atom *a;
	for (a = atoms->a; a && a->next; a = a->next);
	if (a->d || a->a)
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

/*
 * I fucking hate this shit.
 * Any other way would be better.
 */
void init_default_functions() {
  function *f;

  functions = malloc(sizeof(function));
  functions->name = "+";
  functions->function = addfunction;
  functions->next = NULL;
  f = functions;
  
  f->next = malloc(sizeof(function));
  f = f->next;
  f->name = "-";
  f->function = subfunction;
  f->next = NULL;

  f->next = malloc(sizeof(function));
  f = f->next;
  f->name = "*";
  f->function = mulfunction;
  f->next = NULL;

  f->next = malloc(sizeof(function));
  f = f->next;
  f->name = "/";
  f->function = divfunction;
  f->next = NULL;

  f->next = malloc(sizeof(function));
  f = f->next;
  f->name = "=";
  f->function = equalfunction;
  f->next = NULL;

  f->next = malloc(sizeof(function));
  f = f->next;
  f->name = "int?";
  f->function = isint;
  f->next = NULL;

  f->next = malloc(sizeof(function));
  f = f->next;
  f->name = "float?";
  f->function = isfloat;
  f->next = NULL;

  f->next = malloc(sizeof(function));
  f = f->next;
  f->name = "list?";
  f->function = islist;
  f->next = NULL;

  f->next = malloc(sizeof(function));
  f = f->next;
  f->name = "nil?";
  f->function = isnil;
  f->next = NULL;

  f->next = malloc(sizeof(function));
  f = f->next;
  f->name = "cons";
  f->function = consfunction;
  f->next = NULL;

  f->next = malloc(sizeof(function));
  f = f->next;
  f->name = "car";
  f->function = carfunction;
  f->next = NULL;

  f->next = malloc(sizeof(function));
  f = f->next;
  f->name = "cdr";
  f->function = cdrfunction;
  f->next = NULL;

  f->next = malloc(sizeof(function));
  f = f->next;
  f->name = "cond";
  f->function = condfunction;
  f->next = NULL;

  f->next = malloc(sizeof(function));
  f = f->next;
  f->name = "defun";
  f->function = defunfunction;
  f->next = NULL;

  lispfunctions = NULL;
}

void read_files(char **argv, int argc) {
  FILE *f;
  int i;
  
  for (i = 1; i < argc; i++) {
    printf("Evaluating '%s'\n", argv[i]);
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
  int c, d, open, close;
  
  while (1) {
    if (print) printf("-> ");
    
    open = close = 0;
    string[0] = '\0';
    line[0] = '\0';
    
    do {
      if (!fgets(line, sizeof(char) * 500, in))
	return;
      for (c = 0; line[c] && line[c] != '\n'; c++);
      if (line[c] == '\n') line[c] = ' ';

      for (c = 0; string[c]; c++);
      for (d = 0; line[d]; d++)
	string[c + d] = line[d];
      string[c + d] = '\0';
      
      for (c = 0; line[c]; c++)
	if (line[c] == '(') open++;
	else if (line[c] == ')') close++;
      
    } while (open != close);

    parsed = parse(string);
    result = evaluate(parsed);
    if (print) printf("%s\n", atom_to_string(result));
  }
}

int main(int argc, char **argv) {
  init_default_functions();

  if (argc > 1) read_files(argv, argc);
  repl(stdin, 1);
  return 0;
}
