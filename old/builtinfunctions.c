atom *add_function(symbol *symbols, atom *a) {
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

atom *sub_function(symbol *symbols, atom *a) {
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

atom *mul_function(symbol *symbols, atom *a) {
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

atom *div_function(symbol *symbols, atom *a) {
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

atom *equal_function(symbol *symbols, atom *atoms) {
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
				atom *r = equal_function(symbols, a);
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
			r = equal_function(symbols, tmp);
			if (!r->d && !r->f && !r->s)
				return NIL;

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

			r = equal_function(symbols, tmp);
			if (!r->d && !r->f && !r->s)
				return NIL;
		}
	}

	return TRUE;
}

atom *less_than_function(symbol *symbols, atom *atoms) {
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

atom *greater_than_function(symbol *symbols, atom *atoms) {
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

atom *is_int(symbol *symbols, atom *a) {
	if (a && a->d && a->d->type == INT)
		return TRUE;
	return NIL;
}

atom *is_float(symbol *symbols, atom *a) {
	if (a && a->d && a->d->type == FLOAT)
		return TRUE;
	return NIL;
}

atom *is_char(symbol *symbols, atom *a) {
	if (a && a->d && a->d->type == CHAR)
		return TRUE;
	return NIL;
}

atom *to_int(symbol *symbols, atom *a) {
	if (!a->d)
		return NIL;
	if (a->d->type == CHAR || a->d->type == INT) {
		return data_to_atom(int_to_data(a->d->i));
	} else if (a->d->type == FLOAT)
		return data_to_atom(int_to_data((int) a->d->f));

	return NIL;
}

atom *to_float(symbol *symbols, atom *a) {
	if (a->d)
		return data_to_atom(float_to_data((float) a->d->i));
	return NIL;
}

atom *to_char(symbol *symbols, atom *a) {
	if (a->d)
		return data_to_atom(char_to_data((char) a->d->i));
	return NIL;
}

atom *is_list(symbol *symbols, atom *a) {
	if (!a || !a->s) return NIL;
	else return TRUE;
}

atom *is_nil(symbol *symbols, atom *a) {
	if (!a || (a && !a->d && !a->s && !a->f))
		return TRUE;
	else
		return NIL;
}

atom *list_function(symbol *symbols, atom *atoms) {
	return copy_atom(atoms);
}

atom *cons_function(symbol *symbols, atom *atoms) {
	atom *b, *r;
	r = copy_atom(atoms);
	b = copy_atom(atoms->next->s);
	if (!b)
		b = NIL;
	r->next = b;

	return r;
}

atom *car_function(symbol *symbols, atom *atoms) {
	atom *r;

	if (!atoms || !atoms->s) return NIL;
	r = copy_atom(atoms->s);
	r->next = NULL;
	return r;
}

atom *cdr_function(symbol *symbols, atom *atoms) {
	if (!atoms || !atoms->s) return NIL;
	return copy_atom(atoms->s->next);
}

atom *cond_function(symbol *symbols, atom *atoms) {
	atom *sub, *cond, *func;
	atom *conde;
	for (sub = atoms; sub; sub = sub->next) {
		cond = sub->s;
		func = cond->next;
		cond->next = NULL;
		func->next = NULL;

		conde = evaluate(symbols, cond);
		// It isn't this one.
		if (!conde || (!conde->d && !conde->s && !conde->f)) {
			continue;
		}

		atom *funce = evaluate(symbols, func);
		return funce;
	}

	return NIL;
}

atom *lambda_function(symbol *symbols, atom *atoms) {
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

atom *define_function(symbol *symbols, atom *atoms) {
	char *name;
	symbol *s, *nextsave;

	if (!atoms->sym)
		return NIL;

	name = atoms->sym;
	atoms = atoms->next;
	atoms->next = NULL;

	for (s = symbols; s && s->next && strcmp(s->next->name, name); s = s->next);
	if (!s->next) {
		s->next = malloc(sizeof(symbol));
		s->next->next = NULL;
	}

	s->next->name = copy_string(name);
	s->next->atoms = copy_atom(atoms);

	return TRUE;
}

atom *eval_function(symbol *symbols, atom *atoms) {
	char *string;
	atom *a;
	int i;

	string = atom_string_to_string(atoms);
	if (!string)
		return NIL;

	a = parse(string);
	return evaluate(symbols, a);
}

atom *include_function(symbol *symbols, atom *atoms) {
	char *filename;
	FILE *f;

	filename = atom_string_to_string(atoms);
	if (!filename)
		return NIL;

	f = fopen(filename, "r");
	if (!f) {
		printf("COULD NOT INCLUDE FILE: %s\n", filename);
		return NIL;
	}

	repl(symbols, f, 0);

	return TRUE;
}

atom *do_lisp_function(symbol *symbols, atom *fa, atom *atoms) {
	int i;
	atom *func, *a, *s;

	if (!fa || !fa->f)
		return NIL;

	func = swap_in_args(copy_atom(fa->f->function), fa->f->args, atoms);

	atom *funce = evaluate(symbols, func);
	return funce;
}

atom *exit_function(symbol *symbols, atom *atoms) {
	exit_repl = 1;
	return NIL;
}

atom *progn_function(symbol *symbols, atom *atoms) {
	atom *a, *t;
	for (a = atoms; a; a = a->next) {
		if (a->s) {
			t = a->next;
			a->next = NULL;
			evaluate(symbols, a);
			a->next = t;
		}
	}
	return TRUE;
}

atom *swap_in_args(atom *func, atom *args, atom *atoms) {
	atom *a, *b, *c, *tmp;
	int i, j;
	for (a = func; a; a = a->next) {
		if (a->sym) {
			for (i = 0, b = args; b; b = b->next, i++) {
				if (b->sym && strcmp(a->sym, b->sym) == 0) {
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

symbol *init_functions(symbol *symbols, built_in_function functions[], int fn) {
	int i;
	function *f;
	symbol *s;
	atom *a;

	for (s = symbols; s && s->next; s = s->next);
	if (!s) {
		symbols = malloc(sizeof(symbol));
		s = symbols;
	} else {
		s->next = malloc(sizeof(symbol));
		s = s->next;
	}

	for (i = 0; i < fn; i++) {
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

	return symbols;
}
