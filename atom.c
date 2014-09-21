struct function {
  atom *args;
  int argc;
  atom *atoms;
  atom *function;
  atom *(*c_function)(atom *atoms);
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

atom *nil_atom() {
  atom *n = malloc(sizeof(atom));
  n->next = NULL;
  n->d = NULL;
  n->s = NULL;
  n->f = NULL;
  n->sym = NULL;
  return n;
}
