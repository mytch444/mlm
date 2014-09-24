atom *evaluate(atom *raw) {
  int argc, i;
  atom *atoms, *a, *r, *args;

  atoms = swap_symbols(raw);
  
  if (!atoms) return NIL;
  
  if (atoms->s)
    atoms = do_sub(atoms);

  if (atoms->f) {
    if (!atoms->f->function && !atoms->f->c_function)
      return atoms;

    args = atoms->f->atoms;
    for (a = args; a && a->next; a = a->next);
    if (a)
      a->next = atoms->next;
    else
      args = atoms->next;

    argc = atoms->f->argc;
    for (i = 0, a = args; a; a = a->next, i++);
    
    if (argc > 0 && i > argc) {
      printf("too many arguments: %i > %i\n", i, argc);
      atom *prev = NULL;
      for (i = 0, a = args; a; prev = a, a = a->next, i++) {
	if (i >= argc) {
	  prev->next = NULL;
	  break;
	}
      }

    } else if (i < argc || (argc < 0 && i < -argc)) {
      printf("Not enough arguments: %i < %i\n", i, argc);
      atoms->f->atoms = args;
      atoms->next = NULL;

      return atoms;
    }

    if (atoms->f->flat) {
      a = flatten(args);
    } else
      a = args;
      
    if (atoms->f->c_function) {
      if (atoms->f->accept_dirty || isclean(a)) {
	r = atoms->f->c_function(a);
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
  r->c_function = f->c_function;
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
