
atom *addfunction(atom *handle, atom *atoms);
atom *subfunction(atom *handle, atom *atoms);
atom *mulfunction(atom *handle, atom *atoms);
atom *divfunction(atom *handle, atom *atoms);

atom *equalfunction(atom *handle, atom *atoms);
atom *isint(atom *handle, atom *atoms);
atom *isfloat(atom *handle, atom *atoms);
atom *islist(atom *handle, atom *atoms);
atom *isnil(atom *handle, atom *atoms);

atom *consfunction(atom *handle, atom *atoms);
atom *carfunction(atom *handle, atom *atoms);
atom *cdrfunction(atom *handle, atom *atoms);

atom *condfunction(atom *handle, atom *atoms);

atom *swap_in_args(atom *func, atom *args, atom *atoms);
atom *defunfunction(atom *handle, atom *atoms);
atom *dolispfunction(atom *handle, atom *atoms);

atom *addfunction(atom *handle, atom *a) {
  atom *r;
  if (!a) return int_to_atom(0);
  if (a->a) {
    atom *s = evaluate(a->a);
    if (s->next) {
      printf("Error, lists are not my speciality\n");
      return int_to_atom(0);
    }
    s->next = a->next;
    return addfunction(handle, s);
  } else if (a->d) {
    data *d = a->d;
    if (d->type == INT) {
      atom *n = addfunction(handle, a->next);
      r = int_to_atom(d->i + n->d->i);
    } else if (d->type == FLOAT) {
      atom *n = addfunction(handle, a->next);
      r = float_to_atom(d->f + n->d->f);
    } else return int_to_atom(0);
    return r;
  } else return int_to_atom(0);
}

atom *subfunction(atom *handle, atom *a) {
  atom *r, *b;
  if (!a) return int_to_atom(0);
  if (a->a) {
    atom *s = evaluate(a->a);
    if (s->next) {
      printf("Error, lists are not my speciality\n");
      return int_to_atom(0);
    }
    s->next = a->next;
    return subfunction(handle, s);
  } else if (a->d) {
    data *d = a->d;
    if (d->type == INT) {
      int sum = d->i;
      for (b = a->next; b && b->d; b = b->next)
	sum -= b->d->i;

      r = int_to_atom(sum);
    } else if (d->type == FLOAT) {
      float sum = d->f;
      for (b = a->next; b && b->d; b = b->next)
	sum -= b->d->f;

      r = float_to_atom(sum);
    } else return int_to_atom(0);
    return r;
  } else return int_to_atom(0);
}

atom *mulfunction(atom *handle, atom *a) {
  atom *r;
  if (!a) return int_to_atom(1);
  if (a->a) {
    atom *s = evaluate(a->a);
    if (s->next) {
      printf("Error, lists are not my speciality\n");
      return int_to_atom(0);
    }
    s->next = a->next;
    return mulfunction(handle, s);
  } else if (a->d) {
    data *d = a->d;
    if (d->type == INT) {
      atom *n = mulfunction(handle, a->next);
      r = int_to_atom(d->i * n->d->i);
    } else if (d->type == FLOAT) {
      atom *n = mulfunction(handle, a->next);
      r = float_to_atom(d->f * n->d->f);
    } else return int_to_atom(1);
    return r;
  } else return int_to_atom(1);
}

atom *divfunction(atom *handle, atom *a) {
  atom *r, *b;
  if (!a) return int_to_atom(1);
  if (a->a) {
    atom *s = evaluate(a->a);
    if (s->next) {
      printf("Error, lists are not my speciality\n");
      return int_to_atom(0);
    }
    s->next = a->next;
    return divfunction(handle, s);    
  } else if (a->d) {
    data *d = a->d;
    if (d->type == INT) {
      int sum = d->i;
      for (b = a->next; b && b->d; b = b->next)
	sum /= b->d->i;

      r = int_to_atom(sum);
    } else if (d->type == FLOAT) {
      float sum = d->f;
      for (b = a->next; b && b->d; b = b->next)
	sum /= b->d->f;

      r = float_to_atom(sum);
    } else return int_to_atom(1);
    return r;
  } else return int_to_atom(1);
}

atom *equalfunction(atom *handle, atom *atoms) {
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
      } else if (d->type == FUNCTION && n->type == FUNCTION) {
	if (strcmp(d->string, n->string) != 0)
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
	  atom *r = equalfunction(handle, a);
	  if (r->d->i == 0) return NIL;
	  a->next = tmpa;
	  b->next = tmpb;
	}
      }
    } else if (a->a) {
      atom *s = evaluate(a->a);
      if (s->next) {
	a->d = NULL;
	a->a = s;
      } else {
	a->d = s->d;
	a->a = s->a;
      }

      return equalfunction(handle, a);
    } else if (a->next->d || a->next->a) {
      return NIL;
    } else
      return NIL;
  }
  
  return TRUE;
}

atom *isint(atom *handle, atom *atoms) {
  if (atoms && atoms->d && atoms->d->type == INT)
    return TRUE;
  else
    return NIL;
}

atom *isfloat(atom *handle, atom *atoms) {
  if (atoms && atoms->d && atoms->d->type == FLOAT)
    return TRUE;
  else
    return NIL;
}

atom *islist(atom *handle, atom *a) {
  if (!a) return NIL;
  else if (a->a) return TRUE;
  else return NIL;
}

atom *isnil(atom *handle, atom *atoms) {
  if (atoms && !atoms->d && !atoms->a) return TRUE;
  else return NIL;
}

atom *consfunction(atom *handle, atom *atoms) {
  atom *a, *b;
  a = atoms;
  b = evaluate(atoms->next->a);
  a->next = NULL;
  a = evaluate(a);

  a->next = b;
  return a;
}

atom *carfunction(atom *handle, atom *atoms) {
  if (!atoms || !atoms->a) return NIL;
  atoms->a->next = NULL;
  return atoms->a;
}

atom *cdrfunction(atom *handle, atom *atoms) {
  if (!atoms || !atoms->a) return NIL;
  return atoms->a->next;
}

atom *condfunction(atom *handle, atom *atoms) {
  atom *sub, *cond, *func, *tmpnext;
  atom *conde;
  for (sub = atoms; sub; sub = sub->next) {
    tmpnext = sub->next;
    sub->next = NULL;
    sub->next = tmpnext;
    
    cond = sub->a;
    func = cond->next;
    cond->next = NULL;
    func->next = NULL;

    conde = evaluate(cond);
    // It isn't this one.
    if (!conde->d && !conde->a) {
      continue;
    }

    return evaluate(func);
  }

  return NIL;
}

atom *defunfunction(atom *handle, atom *atoms) {
  int argc;
  function *f;
  lispfunction *l;

  if (!atoms || !atoms->d ||
      !atoms->next || !atoms->next->a ||
      !atoms->next->next || !atoms->next->next->a)
    return NIL;
  
  char *name = atoms->d->string;
  atom *args = atoms->next->a;
  atom *func = atoms->next->next->a;
  atom *a;

  printf("name = '%s'\nargs = '%s'\nfunc = '%s'\n", name, atom_to_string(args), atom_to_string(func));
  
  for (argc = 0, a = args; a; a = a->next, argc++);

  for (f = functions; f && f->next; f = f->next);    
  f->next = malloc(sizeof(function));
  f = f->next;
  f->name = name;
  f->function = dolispfunction;
  f->next = NULL;

  for (l = lispfunctions; l && l->next; l = l->next);
  if (!l) {
    lispfunctions = malloc(sizeof(lispfunction));
    l = lispfunctions;
  } else {
    l->next = malloc(sizeof(lispfunction));
    l = l->next;
  }
  l->name = name;
  l->args = args;
  l->argc = argc;
  l->function = func;
  l->next = NULL;
  
  return TRUE;
}

atom *swap_in_args(atom *func, atom *args, atom *atoms) {
  atom *a, *b, *c, *tmp;
  int i, j;
  for (a = func; a; a = a->next) {
    if (a->d && a->d->type == FUNCTION) {
      tmp = a->next;
      a->next = NULL;
      a->next = tmp;
      for (i = 0, b = args; b; b = b->next, i++) {
	if (b->d && strcmp(a->d->string, b->d->string) == 0) {
	  tmp = b->next;
	  b->next = NULL;
	  b->next = tmp;
	  for (j = 0, c = atoms; c; c = c->next, j++) {
	    if (i == j) {
	      tmp = c->next;
	      c->next = NULL;
	      c->next = tmp;

	      a->d = c->d;
	      a->a = c->a;
	      break;
	    }
	  }
	  break;
	}
      }
    } else if (a->a) {
      atom *sub = swap_in_args(a->a, args, atoms);
      a->a = sub;
    }
  }

  return func;
}

atom *dolispfunction(atom *handle, atom *atoms) {
  lispfunction *l;
  int f, i;
  atom *args, *func, *a, *s;

  for (l = lispfunctions; l; l = l->next) {
    if (strcmp(l->name, handle->d->string) == 0) {
      break;
    }
  }

  if (!l)
    return NIL;

  for (i = 0, a = atoms; a; a = a->next, i++) {
    if (a->a) {
      s = evaluate(a->a);
      if (s->next) {
	a->d = NULL;
	a->a = s;
      } else {
	a->d = s->d;
	a->a = s->a;
      }	
    }
  }

  if (i < l->argc) {
    return handle;
  }
  
  args = l->args;
  func = swap_in_args(copy_atom(l->function), args, atoms);
  
  return evaluate(func);
}
