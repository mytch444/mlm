#define err(e, str) { fprintf(stderr, "%s\n", str); return e; }

#define NEW_GENERIC_OPERATOR(name, op) \
int name(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols) \
{ \
	struct mlm_thing * a; \
	printf("doing shit\n"); \
	while (args->type == LST) \
	{ \
		printf("calling eval\n"); \
		a = eval_thing(args->car, symbols); \
		if (r->type == NIL) \
		{ \
			printf("setting type\n"); \
			r->type = a->type;  \
			if (r->type == FLT) r->point = a->point; \
			else r->value = a->value; \
		} else if (a->type == FLT) r->point op a->point; \
		else r->value op a->value; \
		printf("freeing thing\n"); \
		free_thing(a); \
		printf("moving on\n"); \
		args = args->cdr; \
	} \
	return 0; \
}

NEW_GENERIC_OPERATOR(operator_add, +=)

NEW_GENERIC_OPERATOR(operator_sub, -=)

NEW_GENERIC_OPERATOR(operator_mul, *=)

NEW_GENERIC_OPERATOR(operator_div, /=)

#define NEW_INT_OPERATOR(name, op) \
int name(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols) \
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

int operator_equal(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols)
{
	struct mlm_thing * first, * a;
	r->type = INT;
	r->value = 1;
	if (args->type != LST) err(1, "= has no arguments");
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

int operator_greater(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols)
{
	struct mlm_thing * first, * a;
	r->type = INT;
	r->value = 1;
	if (args->type != LST || args->cdr->type != LST)
		err(1, "> has insufficient arguments.");
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

int operator_is(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols)
{

	struct mlm_thing * first, * a;
	r->type = INT;
	r->value = 1;
	if (args->type != LST) return 1;
	first = eval_thing(args->car, symbols);
	args = args->cdr;
	while (args->type == LST && r->type == INT)
	{
		a = eval_thing(args->car, symbols);
		if (first->type != a->type)
			r->type = NIL;
		free_thing(a);
		args = args->cdr;
	}
	free_thing(first);
	return 0;
}

int operator_car(struct mlm_thing *r, struct mlm_thing * args, struct mlm_symbol * symbols)
{
	if (args->type != LST) err(1, "car has no arguments.");
	struct mlm_thing * t = eval_thing(args->car, symbols);
	if (t->type != LST) err(2, "car argument not a list.");
	copy_thing(r, t->car);
	free_thing(t);
	return 0;
}

int operator_cdr(struct mlm_thing *r, struct mlm_thing * args, struct mlm_symbol * symbols)
{
	if (args->type != LST) err(1, "cdr has no arguments.");
	struct mlm_thing * t = eval_thing(args->car, symbols);
	if (t->type != LST) err(2, "cdr argument not a list.");
	copy_thing(r, t->cdr);
	free_thing(t);
	return 0;
}

int operator_cons(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols)
{
	if (args->type != LST || args->cdr->type != LST) err(1, "cons has insufficient arguments!");
	struct mlm_thing * cdr = eval_thing(args->cdr->car, symbols);
	if (cdr->type != LST && cdr->type != NIL) err(2, "cons has bad arguments!");

	r->type = LST;
	r->car = eval_thing(args->car, symbols);
	r->cdr = cdr;
	return 0;
}

int operator_cond(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols)
{
	struct mlm_thing * a = NULL, * b;
	for (; args->type == LST; args = args->cdr)
	{
		if (a) free_thing(a);
		if (args->car->type != LST || args->car->cdr->type != LST)
			err(1, "cond has bad arguments.");
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

int operator_def(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * v)
{
	struct mlm_thing * thing;
	if (args->type != LST || args->car->type != SYM ||args->cdr->type != LST)
		err(1, "def has bad arguments.");
	
	thing = malloc(sizeof(struct mlm_thing));
	copy_thing(thing, args->cdr->car);
	
	for (; v && v->next && strcmp(v->next->name, args->car->label); v = v->next);
	if (v->next) free_thing(v->next->thing);
	else
	{
		v->next = malloc(sizeof(struct mlm_symbol));
		v->next->next = NULL;
		v->next->name = malloc(sizeof(char) * (strlen(args->car->label) + 1));
		strcpy(v->next->name, args->car->label);
	}
	
	v->next->thing = thing;
	
	r->type = INT;
	r->value = 1;
	return 0;
}

int operator_lambda(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols)
{
	struct mlm_symbol * s;
	r->type = FNC;
	r->function = malloc(sizeof(struct mlm_function));
	r->function->thing = malloc(sizeof(struct mlm_thing));
	r->function->thing->type = NIL;
	r->function->symbols = s = malloc(sizeof(struct mlm_symbol)); 
	s->name = NULL; s->thing = NULL; s->next = NULL;
	
	for (; args->type == LST; args = args->cdr)
	{
		if (args->cdr->type == NIL)
		{
			copy_thing(r->function->thing, args->car);
		} else if (args->car->type == SYM)
		{
			s->next = malloc(sizeof(struct mlm_symbol));
			s = s->next;
			s->next = NULL;
			s->thing = NULL;
			s->name = malloc(sizeof(char) * (strlen(args->car->label) + 1));
			strcpy(s->name, args->car->label);
		} else
			err(2, "lambda: not a variable name and not last argument.");
	}
	
	return 0;
}
