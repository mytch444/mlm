#define err(str) { fprintf(stderr, "%s\n", str); return 1; }

#define NEW_GENERIC_OPERATOR(name, op) \
int name(struct mlm_thing * r, \
	struct mlm_thing * args, \
	struct mlm_symbol * symbols) \
{ \
	struct mlm_thing * a; \
	while (args->type == LST) \
	{ \
		a = eval_thing(args->car, symbols); \
		if (r->type == NIL) \
		{ \
			r->type = a->type;  \
			if (r->type == FLT) r->point = a->point; \
			else r->value = a->value; \
		} else if (a->type == FLT) r->point op a->point; \
		else r->value op a->value; \
		free_thing(a); \
		args = args->cdr; \
	} \
	return 0; \
}

NEW_GENERIC_OPERATOR(operator_add, +=)

NEW_GENERIC_OPERATOR(operator_sub, -=)

NEW_GENERIC_OPERATOR(operator_mul, *=)

NEW_GENERIC_OPERATOR(operator_div, /=)

#define NEW_INT_OPERATOR(name, op) \
int name(struct mlm_thing * r, \
	struct mlm_thing * args, \
	struct mlm_symbol * symbols) \
{ \
	struct mlm_thing * a; \
	while (args->type == LST) \
	{ \
		a = eval_thing(args->car, symbols); \
		r->value op a->value; \
		r->type = a->type; \
		free_thing(a); \
		args = args->cdr; \
	} \
	return 0; \
}

NEW_INT_OPERATOR(operator_or, |=)

NEW_INT_OPERATOR(operator_and, &=)

NEW_INT_OPERATOR(operator_xor, ^=)

int operator_equal(struct mlm_thing * r,
		struct mlm_thing * args,
		struct mlm_symbol * symbols)
{
	struct mlm_thing * first, * a;
	r->type = INT;
	r->value = 1;
	if (args->type != LST) err("= has no arguments");
	first = eval_thing(args->car, symbols);
	args = args->cdr;
	while (args->type == LST && r->type == INT)
	{
		a = eval_thing(args->car, symbols);
		if (!thing_equivalent(a, first))
			r->type = NIL;
		free_thing(a);
		args = args->cdr;
	}
	free_thing(first);
	return 0;
}

int operator_greater(struct mlm_thing * r,
		struct mlm_thing * args,
		struct mlm_symbol * symbols)
{
	struct mlm_thing * first, * a;
	r->type = INT;
	r->value = 1;
	if (args->type != LST || args->cdr->type != LST)
		err("> has insufficient arguments.");
	first = eval_thing(args->car, symbols);
	while (args->cdr->type == LST && r->type == INT)
	{
		args = args->cdr;
		a = eval_thing(args->car, symbols);
		if ((first->type == FLT && a->point >= first->point)
		    || a->value >= first->value)
			r->type = NIL;
		free_thing(a);
	}
	free_thing(first);
	return 0;
}

int operator_equal_type(struct mlm_thing * r,
		struct mlm_thing * args,
		struct mlm_symbol * symbols)
{

	struct mlm_thing * first, * a;
	r->type = INT;
	r->value = 1;
	if (args->type != LST) return 1;
	first = eval_thing(args->car, symbols);
	for (args = args->cdr;
	     args->type == LST && r->type == INT;
	     args = args->cdr)
	{
		a = eval_thing(args->car, symbols);
		if (first->type != a->type)
			r->type = NIL;
		free_thing(a);
	}
	free_thing(first);
	return 0;
}

int operator_car(struct mlm_thing *r,
		struct mlm_thing * args,
		struct mlm_symbol * symbols)
{
	if (args->type != LST) err("car has no arguments.");
	struct mlm_thing * t = eval_thing(args->car, symbols);
	if (t->type != LST) err("car argument not a list.");
	copy_thing(r, t->car);
	free_thing(t);
	return 0;
}

int operator_cdr(struct mlm_thing *r,
		struct mlm_thing * args,
		struct mlm_symbol * symbols)
{
	if (args->type != LST) err("cdr has no arguments.");
	struct mlm_thing * t = eval_thing(args->car, symbols);
	if (t->type != LST) err("cdr argument not a list.");
	copy_thing(r, t->cdr);
	free_thing(t);
	return 0;
}

int operator_cons(struct mlm_thing * r,
		struct mlm_thing * args,
		struct mlm_symbol * symbols)
{
	if (args->type != LST || args->cdr->type != LST)
		err("cons has insufficient arguments!");
	struct mlm_thing * cdr = eval_thing(args->cdr->car, symbols);
	if (cdr->type != LST && cdr->type != NIL)
		err("cons has bad arguments!");

	r->type = LST;
	r->car = eval_thing(args->car, symbols);
	r->cdr = cdr;
	return 0;
}

int operator_cond(struct mlm_thing * r,
		struct mlm_thing * args,
		struct mlm_symbol * symbols)
{
	struct mlm_thing * a = NULL, * b;
	for (; args->type == LST; args = args->cdr)
	{
		if (a) free_thing(a);
		if (args->car->type != LST || args->car->cdr->type != LST)
			err("cond has bad arguments.");
		a = eval_thing(args->car->car, symbols);
		if (a->type == NIL) continue;

		b = eval_thing(args->car->cdr->car, symbols);
		copy_thing(r, b);
		free_thing(a);
		free_thing(b);
		break;
	}
	return 0;
}

int operator_def(struct mlm_thing * r,
		struct mlm_thing * args,
		struct mlm_symbol * v)
{
	struct mlm_thing * thing;
	if (args->type != LST 
		|| args->car->type != SYM
		||args->cdr->type != LST)
		err("def has bad arguments.");
	
	thing = eval_thing(args->cdr->car, v);
	add_definition(v, args->car->label, thing);	
	
	r->type = INT;
	r->value = 1;
	return 0;
}

int operator_lambda(struct mlm_thing * r,
		struct mlm_thing * a,
		struct mlm_symbol * symbols)
{
	if (a->type != LST || a->cdr->type != LST)
		err("lambda bad arguments.");
	
	struct mlm_symbol * s;

	r->type = FNC;
	r->function = malloc(sizeof(struct mlm_function));
	
	r->function->thing = malloc(sizeof(struct mlm_thing));
	copy_thing(r->function->thing, a->cdr);
	
	r->function->symbols = s = malloc(sizeof(struct mlm_symbol)); 
	s->name = NULL; s->thing = NULL; s->next = NULL;
	
	for (a = a->car; a->type == LST; a = a->cdr)
	{
		s->next = malloc(sizeof(struct mlm_symbol));
		s = s->next;
		s->next = NULL;
		s->thing = NULL;
		s->name = malloc(sizeof(char) * (strlen(a->car->label) + 1));
		strcpy(s->name, a->car->label);
	}
	
	return 0;
}

int operator_load(struct mlm_thing * r,
		struct mlm_thing * args,
		struct mlm_symbol * symbols)
{
	if (args->type != LST)
		err("load bad arguments.");
	struct mlm_thing * f, * c;
	char path[256];
	int i;
	
	f = eval_thing(args->car, symbols);
	
	for (i = 0, c = f; c->type == LST; c = c->cdr)
		path[i++] = c->car->value;
	path[i] = '\0';
	free_thing(f);

	i = open(path, O_RDONLY);
	if (i > 0)
	{
		f = parse_file(i, symbols);
		free_thing(f);
	}

	close(i);
	
	if (i < 0) r->type = NIL;
	else r->type = INT;
	r->value = 1;
	return 0;
}

int operator_exec(struct mlm_thing * r,
		struct mlm_thing * args,
		struct mlm_symbol * symbols)
{
	struct mlm_thing * a, * c;
	char *name[20];
	int i, j;
	
	for (j = 0; args->type == LST; args = args->cdr)
	{
		a = eval_thing(args->car, symbols);
		for (i = 0, c = a; c->type == LST; c = c->cdr)
			i++;
		name[j] = calloc(sizeof(char), (i + 1));
		for (i = 0, c = a; c->type == LST; c = c->cdr)
			name[j][i++] = c->car->value;
		name[j++][i] = '\0';
		free_thing(a);
	}
	name[j] = NULL;
	
	i = fork();
	if (!i)
		execvp(name[0], name);
	
	r->type = INT;
	waitpid(i, &j, 0);
	r->value = WEXITSTATUS(j);
	return 0;
}
