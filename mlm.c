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
#define FUNCTION 3

#define TRUE int_to_atom(1)
#define NIL nil_atom()

typedef struct function function;
typedef struct lispfunction lispfunction;
typedef struct atom atom;
typedef struct data data;

struct function {
  char *name;
  atom *(*function)(atom *handle, atom *atoms);
};

struct lispfunction {
  char *name;
  atom *args;
  atom *function;
};

struct atom {
  data *d;
  atom *a;
  atom *next;
};

struct data {
  int type;
  int i;
  double f;
  char *string;
};

void read_files(char **argv, int argc);

atom *parse(char *string);
atom *evaluate(atom *atoms);

char *string_cut(char *string, int start, int stop);
int closing_bracket_pos(char *string, int open);

data *string_to_data(char *string);
data *int_to_data(int i);
data *float_to_data(double f);

atom *float_to_atom(double d);
atom *int_to_atom(int i);

char *atom_to_string(atom *a);

data *copy_data(data *a);
atom *copy_atom(atom *a);

int string_is_int(char *string);
int string_is_float(char *string);

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

data *string_to_data(char *string) {
  if (string_is_int(string)) {
    return int_to_data(atoi(string));
  } else if (string_is_float(string)) {
    return float_to_data(atof(string));
    
  } else {
    data *d = malloc(sizeof(data));
    
    d->type = FUNCTION;
    d->string = string;
    
    return d;
  }
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

atom *int_to_atom(int i) {
  atom *a = malloc(sizeof(atom));
  a->d = int_to_data(i);
  a->a = NULL;
  a->next = NULL;
  return a;
}

atom *float_to_atom(double d) {
  atom *a = malloc(sizeof(atom));
  a->d = float_to_data(d);
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

int function_location(atom *a) {
  int i;
  for (i = 0; strcmp(functions[i].name, ""); i++) {
    if (strcmp(functions[i].name, a->d->string) == 0) {
      return i;
    }
  }
  return -1;
}

atom *evaluate(atom *atoms) {
  int i;
  atom *a;

  if (!atoms) return NIL;

  printf("evaluating '%s'\n", atom_to_string(atoms));
  
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
      i = function_location(atoms);
      if (i == -1)
	return atoms;
      else
	return functions[i].function(atoms, atoms->next);
    } else {
      return atoms;
    }
  } else if (atoms->a) {
    return atoms;
  } else
    return NIL;
}

atom *parse(char *string) {
  int num, i, start, end;
  atom *handle, *atoms;
  
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
      
      atoms->next = malloc(sizeof(atom));
      atoms = atoms->next;
      atoms->d = string_to_data(string_cut(string, start, i));
      atoms->a = NULL;
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

void init_default_functions() {
  functions = malloc(sizeof(struct function) * 15);
  functions[0].name = "+";
  functions[0].function = addfunction;
  functions[1].name = "-";
  functions[1].function = subfunction;
  functions[2].name = "*";
  functions[2].function = mulfunction;
  functions[3].name = "/";
  functions[3].function = divfunction;
  functions[4].name = "=";
  functions[4].function = equalfunction;
  functions[5].name = "int?";
  functions[5].function = isint;
  functions[6].name = "float?";
  functions[6].function = isfloat;
  functions[7].name = "list?";
  functions[7].function = islist;
  functions[8].name = "nil?";
  functions[8].function = isnil;
  functions[9].name = "defun";  
  functions[9].function = defunfunction;
  functions[10].name = "cons";
  functions[10].function = consfunction;
  functions[11].name = "car";
  functions[11].function = carfunction;
  functions[12].name = "cdr";
  functions[12].function = cdrfunction;
  functions[13].name = "cond";
  functions[13].function = condfunction;
  functions[14].name = "";
  functions[14].function = NULL;
  
  lispfunctions = malloc(sizeof(struct lispfunction) * 1);
  lispfunctions[0].name = "";
  lispfunctions[0].args = NULL;
  lispfunctions[0].function = NULL;
}

void read_files(char **argv, int argc) {
  FILE *f;
  int i;
  char line[LINELENGTH];
  atom *result;
  atom *parsed;
  
  for (i = 1; i < argc; i++) {
    printf("Evaluating '%s'\n", argv[i]);
    f = fopen(argv[i], "r");
    if (f == NULL) {
      continue;
    }
    
    while (fgets(line, sizeof line, f)) {
      parsed = parse(line);
      result = evaluate(parsed);
    }

    fclose(f);
  }
}

int main(int argc, char **argv) {
  char line[LINELENGTH];
  atom *result;
  atom *parsed;
  
  init_default_functions();
  
  if (argc > 1) read_files(argv, argc);

  while (1) {
    printf("-> ");
    if (!fgets(line, LINELENGTH, stdin)) {
      printf("Error.\n");
      break;
    }

    parsed = parse(line);
    result = evaluate(parsed);
    printf("%s\n", atom_to_string(result));
  }
  
  return 0;
}
