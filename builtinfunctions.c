atom *addfunction(atom *a) {
  atom *r;
  if (a->s) {
    printf("I don't to lists\n");
    return NIL;
  } else if (a->d && a->next->d) {
    if ((a->d->type == INT || a->d->type == CHAR) && a->next->d->type != FLOAT) {
      r = data_to_atom(int_to_data(a->d->i + a->next->d->i));
      if (a->d->type == CHAR || a->next->d->type == CHAR)
	r->d->type = CHAR;
    } else if (a->d->type == FLOAT) {
      r = data_to_atom(float_to_data(a->d->f + a->next->d->f));
    } else r = data_to_atom(int_to_data(0));
    return r;
  } else return data_to_atom(int_to_data(0));
}

atom *subfunction(atom *a) {
  atom *r;
  if (a->s) {
    printf("I don't to lists\n");
    return NIL;
  } else if (a->d && a->next->d) {
    if ((a->d->type == INT || a->d->type == CHAR) && a->next->d->type != FLOAT) {
      r = data_to_atom(int_to_data(a->d->i - a->next->d->i));
      if (a->d->type == CHAR || a->next->d->type == CHAR)
	r->d->type = CHAR;
    } else if (a->d->type == FLOAT) {
      r = data_to_atom(float_to_data(a->d->f - a->next->d->f));
    } else r = data_to_atom(int_to_data(0));
    return r;
  } else return data_to_atom(int_to_data(0));
}

atom *mulfunction(atom *a) {
  atom *r;
  if (a->s) {
    printf("I don't to lists\n");
    return NIL;
  } else if (a->d && a->next->d) {
    if ((a->d->type == INT || a->d->type == CHAR) && a->next->d->type != FLOAT) {
      r = data_to_atom(int_to_data(a->d->i * a->next->d->i));
      if (a->d->type == CHAR || a->next->d->type == CHAR)
	r->d->type = CHAR;
    } else if (a->d->type == FLOAT) {
      r = data_to_atom(float_to_data(a->d->f * a->next->d->f));
    } else r = data_to_atom(int_to_data(0));
    return r;
  } else return data_to_atom(int_to_data(0));
}

atom *divfunction(atom *a) {
  atom *r;
  if (a->s) {
    printf("I don't to lists\n");
    return NIL;
  } else if (a->d && a->next->d) {
    if ((a->d->type == INT || a->d->type == CHAR) && a->next->d->type != FLOAT) {
      r = data_to_atom(int_to_data(a->d->i / a->next->d->i));
      if (a->d->type == CHAR || a->next->d->type == CHAR)
	r->d->type = CHAR;
    } else if (a->d->type == FLOAT) {
      r = data_to_atom(float_to_data(a->d->f / a->next->d->f));
    } else r = data_to_atom(int_to_data(0));
    return r;
  } else return data_to_atom(int_to_data(0));
}

atom *equalfunction(atom *atoms) {
  atom *a = atoms;
  atom *b = atoms->next;
  
  if (a->d && b->d) {
    if (a->d->i != b->d->i || a->d->f != b->d->f)
      return NIL;
  } else if (a->s && b->s) {
    for (a = a->s, b = b->s; 1; a = a->next, b = b->next) {
      if (!a && !b) break;
      else if (!a || !b) return NIL;
      else {
	atom *tmpa = a->next;
	a->next = b;
	atom *r = equalfunction(a);
	a->next = tmpa;
	if (!r->d && !r->f && !r->s)
	  return NIL;
      }
    }  
  } else if (a->f && b->f) {
    if (a->f->c_function && b->f->c_function) {
      if (a->f->c_function != b->f->c_function)
	return NIL;
    } else if (a->f->function && b->f->function) {
      atom *tmp, *r;
      
      // Check if the functoin is equal.
      tmp = a->f->function;
      tmp->next = b->f->function;
      r = equalfunction(tmp);
      if (!r->d && !r->f && !r->s)
	return NIL;

      printf("Checking atoms\n");

      // Check if the atoms are equal.
      tmp = malloc(sizeof(atom));
      tmp->d = NULL;
      tmp->f = NULL;
      tmp->sym = NULL;
      tmp->s = a->f->atoms;
      tmp->next = malloc(sizeof(atom));
      tmp->d = NULL;
      tmp->f = NULL;
      tmp->sym = NULL;
      tmp->s = b->f->atoms;
      tmp->next = NULL;
      printf("Checking '%s'\n", atom_to_string(tmp));
      r = equalfunction(tmp);
      if (!r->d && !r->f && !r->s)
	return NIL;
      printf("all good\n");
    }
  }
  
  return TRUE;
}

atom *lessthanfunction(atom *atoms) {
  atom *a = atoms;
  atom *b = atoms->next;
  
  if (a->d && b->d) {
    if (
	((a->d->type == INT || a->d->type == CHAR) && a->d->i >= b->d->i)
	|| ((a->d->type == FLOAT) && a->d->f >= b->d->f))
      return NIL;
  } else if (a->s && b->s) {
    int as, bs;
    for (as = 0, a = a->s; a; a = a->next, as++);
    for (bs = 0, b = b->s; b; b = b->next, bs++);

    if (as >= bs)
      return NIL;
  } else
    return NIL;
  
  return TRUE;
}

atom *greaterthanfunction(atom *atoms) {
  atom *a = atoms;
  atom *b = atoms->next;
  
  if (a->d && b->d) {
    if (
	((a->d->type == INT || a->d->type == CHAR) && a->d->i <= b->d->i)
	|| ((a->d->type == FLOAT) && a->d->f <= b->d->f))
      return NIL;
  } else if (a->s && b->s) {
    int as, bs;
    for (as = 0, a = a->s; a; a = a->next, as++);
    for (bs = 0, b = b->s; b; b = b->next, bs++);

    if (as <= bs)
      return NIL;
  } else
    return NIL;
  
  return TRUE;
}

atom *isint(atom *a) {
  if (a && a->d && a->d->type == INT)
    return TRUE;
  else
    return NIL;
}

atom *isfloat(atom *a) {
  if (a && a->d && a->d->type == FLOAT)
    return TRUE;
  else
    return NIL;
}

atom *islist(atom *a) {
  if (!a || !a->s) return NIL;
  else return TRUE;
}

atom *isnil(atom *a) {
  if (!a || (a && !a->d && !a->s && !a->f))
    return TRUE;
  else
    return NIL;
}

atom *listfunction(atom *atoms) {
  return atoms;
}

atom *consfunction(atom *atoms) {
  atom *b, *r;
  r = copy_atom(atoms);
  b = copy_atom(atoms->next->s);
  if (!b)
    b = NIL;
  r->next = b;

  return r;
}

atom *carfunction(atom *atoms) {
  atom *r;
  
  if (!atoms || !atoms->s) return NIL;
  r = copy_atom(atoms->s);
  r->next = NULL;
  return r;
}

atom *cdrfunction(atom *atoms) {
  if (!atoms || !atoms->s) return NIL;
  return copy_atom(atoms->s->next);
}

atom *condfunction(atom *atoms) {
  atom *sub, *cond, *func;
  atom *conde;
  for (sub = atoms; sub; sub = sub->next) {
    cond = sub->s;
    func = cond->next;
    cond->next = NULL;
    func->next = NULL;

    conde = evaluate(cond);
    // It isn't this one.
    if (!conde || (!conde->d && !conde->s && !conde->f)) {
      continue;
    }

    atom *funce = evaluate(func);
    return funce;
  }

  return NIL;
}

atom *printfunction(atom *atoms) {
  atom *ca, *a, *r, *copy;
  char string[1000], c, *sub;
  int i, j;

  if (!atoms->s)
    return NIL;

  a = atoms->next;

  string[0] = '\0';  
  for (i = 0, ca = atoms->s; ca; ca = ca->next) {
    if (!ca->d || ca->d->type != CHAR)
      break;

    c = ca->d->i;

    if (c == '%') {
      if (ca->next && ca->next->d && ca->next->d->type == CHAR && ca->next->d->i == '%') {
	string[i++] = '%';
	string[i] = '\0';
	continue;
      }
	
      if (!a)
	continue;

      copy = copy_atom(a);
      copy->next = NULL;
      
      sub = atom_to_string(copy);
      a = a->next;

      for (j = 0; sub[j]; j++)
	string[i + j] = sub[j];
      string[i + j] = '\0';
      
      i += j;
      
    } else {
      string[i++] = c;
      string[i] = '\0';
    }
  }

  printf("%s\n", string);
  return TRUE;
}

atom *lambdafunction(atom *atoms) {
  int argc;
  function *f;
  atom *args, *func, *a, *r;

  if (!atoms || !atoms->next)
    return NIL;

  args = copy_atom(atoms->s);
  for (argc = 0, a = args; a; a = a->next, argc++);
  func = copy_atom(atoms->next);
  func->next = NULL;

  f = malloc(sizeof(function));
  f->args = args;
  f->argc = argc;
  f->atoms = NULL;
  f->function = func;
  f->c_function = NULL;
  f->accept_dirty = 0;
  f->flat = 1;

  r = malloc(sizeof(atom));
  r->next = NULL;
  r->sym = NULL;
  r->s = NULL;
  r->d = NULL;
  r->f = f;

  return r;
}

atom *definefunction(atom *atoms) {
  char *name;
  symbol *s, *nextsave;
  
  if (!atoms->sym)
    return NIL;

  name = atoms->sym->name;
  atoms = atoms->next;
  atoms->next = NULL;

  for (s = symbols; s && s->next && strcmp(s->next->name, name); s = s->next);
  if (!s->next) {
    s->next = malloc(sizeof(symbol));
    s->next->next = NULL;
  } else
    printf("REDEFINING '%s'\n", s->next->name);

  s->next->name = name;

  s->next->atoms = update_symbols(atoms);

  printf("DEFINED '%s' TO '%s'\n", name, atom_to_string(s->next->atoms));

  return TRUE;
}

atom *do_lisp_function(atom *fa, atom *atoms) {
  int i;
  atom *func, *a, *s;

  if (!fa || !fa->f)
    return NIL;

  func = swap_in_args(copy_atom(fa->f->function), fa->f->args, atoms);

  atom *funce = evaluate(func);
  return funce;
}

atom *swap_in_args(atom *func, atom *args, atom *atoms) {
  atom *a, *b, *c, *tmp;
  int i, j;
  for (a = func; a; a = a->next) {
    if (a->sym) {
      for (i = 0, b = args; b; b = b->next, i++) {
	if (b->sym && strcmp(a->sym->name, b->sym->name) == 0) {
	  for (j = 0, c = atoms; c; c = c->next, j++) {
	    if (i == j) {
	      a->d = c->d;
	      a->s = c->s;
	      a->f = c->f;
	      a->sym = NULL;
	      break;
	    }
	  }
	  break;
	}
      }
    } else if (a->s) {
      a->s = swap_in_args(a->s, args, atoms);
    } else if (a->d) {
      
    }
  }
  
  return func;
}

atom *update_symbols(atom *atoms) {
  atom *a;
  symbol *s;
  for (a = atoms; a; a = a->next) {
    if (a->sym) {
      s = find_symbol(a->sym->name);
      if (s)
	a->sym = s;
    } else if (a->s) {
      a->s = update_symbols(a->s);
    } else if (a->f) {
      a->f->atoms = update_symbols(a->f->atoms);
      a->f->function = update_symbols(a->f->function);
    }
  }

  return atoms;
}

void init_functions(built_in_function functions[], int fn) {
  int i;
  function *f;
  symbol *s;
  atom *a;
 
  symbols = malloc(sizeof(symbol));
  for (i = 0, s = symbols; i < fn; i++) {

    f = malloc(sizeof(function));
    f->args = NULL;
    f->argc = functions[i].argc;
    f->atoms = NULL;
    f->function = NULL;
    f->c_function = functions[i].function;
    f->accept_dirty = functions[i].accept_dirty;
    f->flat = functions[i].flat;

    a = malloc(sizeof(atom));
    a->f = f;
    a->s = NULL;
    a->d = NULL;
    a->sym = NULL;
    a->next = NULL;

    s->name = functions[i].name;
    s->atoms = a;
    s->next = NULL;

    if (i + 1 < fn) {
      s->next = malloc(sizeof(symbol));
      s = s->next;
    }
  }
}
