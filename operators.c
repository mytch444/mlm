#define NEW_GENERIC_OPERATOR(name, op) \
void name(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols) \
{ \
	struct mlm_thing * a; \
	while (args->type == LST) \
	{ \
		a = eval_thing(args->car, symbols); \
		if (r->type == NIL) \
		{ \
			if (a->type == FLT) r->point = a->point; \
			else r->value = a->value; \
		} else if (a->type == FLT) r->point op a->point; \
		else r->value op a->value; \
		r->type = a->type; \
		free_thing(a); \
		args = args->cdr; \
	} \
}

NEW_GENERIC_OPERATOR(operator_add, +=)
MLM_NEW_FUNCTION_THING(operator_add, operator_add_function, operator_add_thing)

NEW_GENERIC_OPERATOR(operator_sub, -=)
MLM_NEW_FUNCTION_THING(operator_sub, operator_sub_function, operator_sub_thing)

NEW_GENERIC_OPERATOR(operator_mul, *=)
MLM_NEW_FUNCTION_THING(operator_mul, operator_mul_function, operator_mul_thing)

NEW_GENERIC_OPERATOR(operator_div, /=)
MLM_NEW_FUNCTION_THING(operator_div, operator_div_function, operator_div_thing)

#define NEW_INT_OPERATOR(name, op) \
void name(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols) \
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
}

NEW_INT_OPERATOR(operator_or, |=)
MLM_NEW_FUNCTION_THING(operator_or, operator_or_function, operator_or_thing)

NEW_INT_OPERATOR(operator_and, &=)
MLM_NEW_FUNCTION_THING(operator_and, operator_and_function, operator_and_thing)

NEW_INT_OPERATOR(operator_xor, ^=)
MLM_NEW_FUNCTION_THING(operator_xor, operator_xor_function, operator_xor_thing)

void operator_equal(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols)
{
	struct mlm_thing * first, * a;
	r->type = INT;
	r->value = 1;
	if (args->type != LST) die("ERROR = has no arguments!");
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
}

MLM_NEW_FUNCTION_THING(operator_equal, operator_equal_function, operator_equal_thing)

void operator_greater(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols)
{
	struct mlm_thing * first, * a;
	r->type = INT;
	r->value = 1;
	if (args->type != LST || args->cdr->type != LST)
		die("ERROR > has to few arguments!");
	first = eval_thing(args->car, symbols);
	while (args->cdr->type == LST && r->type == INT)
	{
		args = args->cdr;
		a = eval_thing(args->car, symbols);
		if (first->type == FLT && a->point >= first->point
			|| a->value >= first->value)
			r->type = NIL;
		free_thing(a);
	}
	free_thing(first);
}

MLM_NEW_FUNCTION_THING(operator_greater, operator_greater_function, operator_greater_thing)

void operator_is(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols)
{

	struct mlm_thing * first, * a;
	r->type = INT;
	r->value = 1;
	if (args->type != LST) die("ERROR is has no arguments!");
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
}

MLM_NEW_FUNCTION_THING(operator_is, operator_is_function, operator_is_thing)

void operator_car(struct mlm_thing *r, struct mlm_thing * args, struct mlm_symbol * symbols)
{
	if (args->type != LST) die("ERROR car arguments nil!");
	struct mlm_thing * t = eval_thing(args->car, symbols);
	if (t->type != LST) die("ERROR car bad arguments!");
	copy_thing(r, t->car);
	free_thing(t);
}

MLM_NEW_FUNCTION_THING(operator_car, operator_car_function, operator_car_thing)

void operator_cdr(struct mlm_thing *r, struct mlm_thing * args, struct mlm_symbol * symbols)
{
	if (args->type != LST) die("ERROR cdr arguments nil!");
	struct mlm_thing * t = eval_thing(args->car, symbols);
	if (t->type != LST) die("ERROR cdr bad arguments!");
	copy_thing(r, t->cdr);
	free_thing(t);
}

MLM_NEW_FUNCTION_THING(operator_cdr, operator_cdr_function, operator_cdr_thing)

void operator_cons(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols)
{
	if (args->type != LST) die("ERROR cons has no arguments!");
	struct mlm_thing * cdr = eval_thing(args->cdr->car, symbols);
	if (cdr->type != LST && cdr->type != NIL) die("ERROR cons has bad arguments!");

	r->type = LST;
	r->car = eval_thing(args->car, symbols);
	r->cdr = cdr;
}

MLM_NEW_FUNCTION_THING(operator_cons, operator_cons_function, operator_cons_thing)

void operator_cond(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols)
{
	struct mlm_thing * a = NULL, * b;
	for (; args->type == LST; args = args->cdr)
	{
		if (a) free_thing(a);
		if (args->car->type != LST || args->car->cdr->type != LST)
			die("ERROR Bad arguments for cond!");
		a = eval_thing(args->car->car, symbols);
		if (a->type == NIL) continue;

		b = eval_thing(args->car->cdr->car, symbols);
		copy_thing(r, b);
		free_thing(a);
		free_thing(b);
		break;
	}
}

MLM_NEW_FUNCTION_THING(operator_cond, operator_cond_function, operator_cond_thing)

void operator_def(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * v)
{
	struct mlm_thing * thing;
	if (args->type != LST || args->car->type != SYM ||args->cdr->type != LST)
		die("ERROR bad arguments for def");
	
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
}

MLM_NEW_FUNCTION_THING(operator_def, operator_def_function, operator_def_thing)

void operator_lambda(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols)
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
			die("ERROR not a variable name and not last argument!\n");
	}
}

MLM_NEW_FUNCTION_THING(operator_lambda, operator_lambda_function, operator_lambda_thing)
