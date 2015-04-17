void operator_add(struct thing * r, struct thing * args, struct variable * variables)
{
	struct thing * a;
	r->value = 0;
	while (args->type == LST)
	{
		a = eval_thing(args->car, variables);
		r->type = a->type;
		r->value += a->value;
		free_thing(a);
		args = args->cdr;
	}
}

void operator_sub(struct thing * r, struct thing * args, struct variable * variables)
{
	struct thing * a;
	while (args->type == LST)
	{
		a = eval_thing(args->car, variables);
		if (r->type == NIL)
			r->value = a->value;
		else r->value -= a->value;
		r->type = a->type;
		free_thing(a);
		args = args->cdr;
	}
}

void operator_mul(struct thing * r, struct thing * args, struct variable * variables)
{
	struct thing * a;
	while (args->type == LST)
	{
		a = eval_thing(args->car, variables);
		if (r->type == NIL)
			r->value = a->value;
		else r->value *= a->value;
		r->type = a->type;
		free_thing(a);
		args = args->cdr;
	}
}

void operator_div(struct thing * r, struct thing * args, struct variable * variables)
{
	struct thing * a;
	while (args->type == LST)
	{
		a = eval_thing(args->car, variables);
		if (r->type == NIL)
			r->value = a->value;
		else r->value /= a->value;
		r->type = a->type;
		free_thing(a);
		args = args->cdr;
	}
}

void operator_equal(struct thing * r, struct thing * args, struct variable * variables)
{
	struct thing * first, * a;
	r->type = INT;
	r->value = 1;
	if (args->type != LST) die("ERROR = has no arguments!");
	first = eval_thing(args->car, variables);
	args = args->cdr;
	while (args->type == LST && r->type == INT)
	{
		a = eval_thing(args->car, variables);
		if (!thing_equivalent(a, first))
			r->type = NIL;
		free_thing(a);
		args = args->cdr;
	}
	free_thing(first);
}

void operator_greater(struct thing * r, struct thing * args, struct variable * variables)
{
	struct thing * first, * a;
	r->type = INT;
	r->value = 1;
	if (args->type != LST || args->cdr->type != LST)
		die("ERROR > has to few arguments!");
	first = eval_thing(args->car, variables);
	while (args->cdr->type == LST && r->type == INT)
	{
		args = args->cdr;
		a = eval_thing(args->car, variables);
		if (a->value >= first->value)
			r->type = NIL;
		free_thing(a);
	}
	free_thing(first);
}

void operator_is(struct thing * r, struct thing * args, struct variable * variables)
{

	struct thing * first, * a;
	r->type = INT;
	r->value = 1;
	if (args->type != LST) die("ERROR is has no arguments!");
	first = eval_thing(args->car, variables);
	args = args->cdr;
	while (args->type == LST && r->type == INT)
	{
		a = eval_thing(args->car, variables);
		if (first->type != a->type)
			r->type = NIL;
		free_thing(a);
		args = args->cdr;
	}
	free_thing(first);
}

void operator_car(struct thing *r, struct thing * args, struct variable * variables)
{
	if (args->type != LST) die("ERROR car arguments nil!");
	struct thing * t = eval_thing(args->car, variables);
	if (t->type != LST) die("ERROR car bad arguments!");
	copy_thing(r, t->car);
	free_thing(t);
}

void operator_cdr(struct thing *r, struct thing * args, struct variable * variables)
{
	if (args->type != LST) die("ERROR cdr arguments nil!");
	struct thing * t = eval_thing(args->car, variables);
	if (t->type != LST) die("ERROR cdr bad arguments!");
	copy_thing(r, t->cdr);
	free_thing(t);
}

void operator_cons(struct thing * r, struct thing * args, struct variable * variables)
{
	if (args->type != LST) die("ERROR cons has no arguments!");
	struct thing * cdr = eval_thing(args->cdr->car, variables);
	if (cdr->type != LST && cdr->type != NIL) die("ERROR cons has bad arguments!");

	r->type = LST;
	r->car = eval_thing(args->car, variables);
	r->cdr = cdr;
}

void operator_cond(struct thing * r, struct thing * args, struct variable * variables)
{
	struct thing * a = NULL, * b;
	for (; args->type == LST; args = args->cdr)
	{
		if (a) free_thing(a);
		if (args->car->type != LST || args->car->cdr->type != LST)
			die("ERROR Bad arguments for cond!");
		a = eval_thing(args->car->car, variables);
		if (a->type == NIL) continue;

		b = eval_thing(args->car->cdr->car, variables);
		copy_thing(r, b);
		free_thing(a);
		free_thing(b);
		break;
	}
}

void operator_define(struct thing * r, struct thing * args, struct variable * v)
{
	struct thing * thing;
	if (args->type != LST || args->car->type != SYM ||args->cdr->type != LST)
		die("ERROR bad arguments for def");
	
	thing = eval_thing(args->cdr->car, v);
	
	for (; v && v->next && strcmp(v->next->label, args->car->label); v = v->next);
	if (v->next) free_thing(v->next->thing);
	else
	{
		v->next = malloc(sizeof(struct variable));
		v->next->next = NULL;
		v->next->label = malloc(sizeof(char) * (strlen(args->car->label) + 1));
		strcpy(v->next->label, args->car->label);
	}
	
	v->next->thing = thing;
	
	r->type = INT;
	r->value = 1;
}

void operator_lambda(struct thing * r, struct thing * args, struct variable * variables)
{
	struct variable * v;
	r->type = FNC;
	r->function = malloc(sizeof(struct function));
	r->function->thing = malloc(sizeof(struct thing));
	r->function->thing->type = NIL;
	r->function->variables = v = malloc(sizeof(struct variable)); 
	v->next = NULL;
	
	for (; args->type == LST; args = args->cdr)
	{
		if (args->cdr->type == NIL)
		{
			copy_thing(r->function->thing, args->car);
		} else if (args->car->type == SYM)
		{
			v->next = malloc(sizeof(struct variable));
			v = v->next;
			v->thing = NULL;
			v->next = NULL;
			v->label = malloc(sizeof(char) * (strlen(args->car->label) + 1));
			strcpy(v->label, args->car->label);
		} else
			die("ERROR not a function or variable name!\n");
	}
	
	
	
	v = r->function->variables;
	r->function->variables = v->next;
	free(v);
}

void operator_include(struct thing * r, struct thing * args, struct variable * variables)
{
	if (args->car->type != LST) die("ERROR bad arguments for include");
	char * filename = char_list_to_string(args->car);
	int fd = open(filename, O_RDONLY);
	if (fd < 0) die("FAILED TO OPEN INCLUDE");
	struct thing * s = malloc(sizeof(struct thing));
	s->type = NIL;
	s = parse_file(fd, s, variables);
	copy_thing(r, s);
	free_thing(s);
}