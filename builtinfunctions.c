atom *addfunction(atom *a) {
  atom *r;
  if (!a || !a->next) return NIL;
  if (a->a) {
    printf("I don't to lists\n");
    return NIL;
  } else if (a->d && a->next->d && a->d->type == a->next->d->type) {
    if (a->d->type == INT || a->d->type == CHAR) {
      r = data_to_atom(int_to_data(a->d->i + a->next->d->i));
    } else if (a->d->type == FLOAT) {
      r = data_to_atom(float_to_data(a->d->f + a->next->d->f));
    } else r = data_to_atom(int_to_data(0));
    return r;
  } else return data_to_atom(int_to_data(0));
}

atom *subfunction(atom *a) {
  atom *r;
  if (!a || !a->next) return NIL;
  if (a->a) {
    printf("I don't to lists\n");
    return NIL;
  } else if (a->d && a->next->d && a->d->type == a->next->d->type) {
    if (a->d->type == INT || a->d->type == CHAR) {
      r = data_to_atom(int_to_data(a->d->i - a->next->d->i));
    } else if (a->d->type == FLOAT) {
      r = data_to_atom(float_to_data(a->d->f - a->next->d->f));
    } else r = data_to_atom(int_to_data(0));
    return r;
  } else return data_to_atom(int_to_data(0));
}

atom *mulfunction(atom *a) {
  atom *r;
  if (!a || !a->next) return NIL;
  if (a->a) {
    printf("I don't to lists\n");
    return NIL;
  } else if (a->d && a->next->d && a->d->type == a->next->d->type) {
    if (a->d->type == INT || a->d->type == CHAR) {
      r = data_to_atom(int_to_data(a->d->i * a->next->d->i));
    } else if (a->d->type == FLOAT) {
      r = data_to_atom(float_to_data(a->d->f * a->next->d->f));
    } else r = data_to_atom(int_to_data(0));
    return r;
  } else return data_to_atom(int_to_data(0));
}

atom *divfunction(atom *a) {
  atom *r;
  if (!a || !a->next) return NIL;
  if (a->a) {
    printf("I don't to lists\n");
    return NIL;
  } else if (a->d && a->next->d && a->d->type == a->next->d->type) {
    if (a->d->type == INT || a->d->type == CHAR) {
      r = data_to_atom(int_to_data(a->d->i / a->next->d->i));
    } else if (a->d->type == FLOAT) {
      r = data_to_atom(float_to_data(a->d->f / a->next->d->f));
    } else r = data_to_atom(int_to_data(0));
    return r;
  } else return data_to_atom(int_to_data(0));
}

atom *equalfunction(atom *atoms) {
  atom *a;
  for (a = atoms; a && a->next; a = a->next) {
    if (!a->next->d && !a->next->a && !a->next->next) break;
    if (a->d && a->next->d) {
      data *d = a->d;
      data *n = a->next->d;

      if (d->type == FLOAT && n->type == FLOAT) {
	if (d->f != n->f)
	  return NIL;
      } else if (d->type == INT && n->type == INT) {
	if (d->i != n->i)
	  return NIL;
      } else
	return NIL;
      
    } else if (a->a && a->next->a) {
      // Not sure if working properly, seems to be but does some funny stuff
      // that could affect further shit.
      atom *suba = a->a;
      atom *subb = a->next->a;

      atom *a, *b;
      for (a = suba, b = subb; 1; a = a->next, b = b->next) {
	if (!a && !b) break;
	else if (a && !b) return NIL;
	else if (!a && b) return NIL;
	else {
	  atom *tmpa = a->next;
	  atom *tmpb = b->next;
	  a->next = b;
	  b->next = NULL;
	  atom *r = equalfunction(a);
	  if (r->d->i == 0) return NIL;
	  a->next = tmpa;
	  b->next = tmpb;
	}
      }
    } else if (a->f && a->next->f) {
      printf("Cannot compare functions at the moment, give me a while. And remind me!\n");
    } else if (a->next->d || a->next->a) {
      return NIL;
    } else
      return NIL;
  }
  
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
  if (!a || !a->a) return NIL;
  else return TRUE;
}

atom *isnil(atom *a) {
  printf("is nil? '%s'\n", atom_to_string(a));
  if (!a || (a && !a->d && !a->a && !a->f)) {
    printf("yes\n");
    return TRUE;
  } else {
    printf("no\n");
    return NIL;
  }
}

atom *listfunction(atom *atoms) {
  return atoms;
}

atom *consfunction(atom *atoms) {
  atom *b, *r;
  r = copy_atom(atoms);
  b = copy_atom(atoms->next->a);
  if (!b)
    b = NIL;
  r->next = b;

  return r;
}

atom *carfunction(atom *atoms) {
  atom *r;
  
  if (!atoms || !atoms->a) return NIL;
  r = copy_atom(atoms->a);
  r->next = NULL;
  return r;
}

atom *cdrfunction(atom *atoms) {
  if (!atoms || !atoms->a) return NIL;
  return atoms->a->next;
}

atom *condfunction(atom *atoms) {
  printf("cond '%s'\n", atom_to_string(atoms));
  atom *sub, *cond, *func;
  atom *conde;
  for (sub = atoms; sub; sub = sub->next) {
    cond = sub->a;
    func = cond->next;
    cond->next = NULL;
    func->next = NULL;

    printf("if '%s' then '%s'\n", atom_to_string(cond), atom_to_string(func));
    
    conde = evaluate(cond);
    // It isn't this one.
    if (!conde || (!conde->d && !conde->a && !conde->f)) {
      printf("nah\n");
      continue;
    }

    printf("yeah\n");

    atom *funce = evaluate(func);
    printf("so '%s'\n", atom_to_string(funce));
    return funce;
  }

  return NIL;
}

void update_function(atom *func, char *name, realfunction *f) {
  atom *a;
  for (a = func; a; a = a->next) {
    if (a->f) {
      if (strcmp(a->f->name, name) == 0)
	a->f->function = f;
    } else if (a->a)
      update_function(a->a, name, f);
  }
}

atom *defunfunction(atom *atoms) {
  int argc;
  realfunction *f;
  char *name;
  atom *args, *func, *a;

  if (!atoms || !atoms->f ||
      !atoms->next || 
      !atoms->next->next)
    return NIL;
  
  name = atoms->f->name;
  args = atoms->next->a;
  for (argc = 0, a = args; a; a = a->next, argc++);
  if (atoms->next->next->a)
    func = atoms->next->next->a;
  else {
    func = atoms->next->next;
    func->next = NULL;
  }

  printf("defining %s\nargs '%s'\nargc %i\nfunc '%s'\n", name, atom_to_string(args), argc, atom_to_string(func));

  for (f = functions; f && f->next && strcmp(f->next->name, name); f = f->next);
  if (!f->next) {
    f->next = malloc(sizeof(realfunction));
    f->next->next = NULL;
  }
  
  f = f->next;
  f->name = name;
  f->args = args;
  f->argc = argc;
  f->function = func;
  f->b_function = NULL;
  f->accept_dirty = 0;
  f->flat = 0;

  update_function(func, name, f);

  return TRUE;
}

atom *swap_in_args(atom *func, atom *args, atom *atoms) {
  //  printf("swapping shit '%s'\n", atom_to_string(func));
  //  printf("args '%s' atoms '%s'\n", atom_to_string(args), atom_to_string(atoms));
  atom *a, *b, *c, *tmp;
  int i, j;
  for (a = func; a; a = a->next) {
    if (a->f) {
      for (i = 0, b = args; b; b = b->next, i++) {
	if (b->f && strcmp(a->f->name, b->f->name) == 0) {
	  for (j = 0, c = atoms; c; c = c->next, j++) {
	    if (i == j) {
	      a->d = c->d;
	      a->a = c->a;
	      a->f = c->f;
	      break;
	    }
	  }
	  break;
	}
      }
    } else if (a->a) {
      a->a = swap_in_args(a->a, args, atoms);
    } else if (a->d) {
    }
  }

  return func;
}

atom *do_lisp_function(atom *fa, atom *atoms) {
  int i;
  atom *func, *a, *s;
  realfunction *f;

  if (!fa || !fa->f)
    return NIL;

  f = fa->f->function;
  if (!f) {
    printf("umm what the fuck is going on\n", fa->f->name);
    return fa;
  }
  
  func = swap_in_args(copy_atom(f->function), f->args, atoms);
  printf("parsed  '%s'\n", atom_to_string(func));

  atom *funce = evaluate(func);
  printf("evaled: '%s'\n", atom_to_string(funce));
  return funce;
}
