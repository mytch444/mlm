void operator_add(struct thing * r, struct thing * args, struct variable * variables)
{
	struct thing * a;
	r->value = 0;
	while (args->type == LST)
	{
		a = eval_thing(args->car, variables);
		if (r->type == NIL) r->type = a->type;
		if (r->type == DBL) r->point += a->point;
		else r->value += a->value;
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
		{
			r->type = a->type;
			if (r->type == DBL) r->point = a->point;
			else r->value = a->value;
		}
		if (r->type == DBL) r->point -= a->point;
		else r->value -= a->value;
		free_thing(a);
		args = args->cdr;
	}
}

void operator_equal(struct thing * r, struct thing * args, struct variable * variables)
{
	struct thing * first, * a;
	r->type = INT;
	r->value = 1;
	if (args->type != LST) return;
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
	if (args->type != LST) return;
	first = eval_thing(args->car, variables);
	args = args->cdr;
	while (args->type == LST && r->type == INT)
	{
		a = eval_thing(args->car, variables);
		if (a->type == DBL && a->point >= first->point)
			r->type = NIL;
		else if (a->value >= first->value)
			r->type = NIL;
		free_thing(a);
		args = args->cdr;
	}
	free_thing(first);
}

void operator_is(struct thing * r, struct thing * args, struct variable * variables)
{

	struct thing * first, * a;
	r->type = INT;
	r->value = 1;
	if (args->type != LST) return;
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
	if (args->type != LST || args->car->type != LST) return;
	copy_thing(r, args->car->car);
}

void operator_cdr(struct thing *r, struct thing * args, struct variable * variables)
{
	if (args->type != LST || args->car->type != LST) return;
	copy_thing(r, args->car->cdr);
}

void operator_cons(struct thing * r, struct thing * args, struct variable * variables)
{
	if (args->type != LST) return;
	struct thing * cdr = eval_thing(args->cdr->car, variables);
	if (cdr->type != LST && cdr->type != NIL)
	{
		free_thing(cdr);
		return;
	}

	r->type = LST;
	r->car = eval_thing(args->car, variables);
	r->cdr = cdr;
}

void operator_cond(struct thing * r, struct thing * args, struct variable * variables)
{
	struct thing * a, * b;
	for (; args->type == LST; args = args->cdr)
	{
		if (a) free_thing(a);
		if (args->car->type != LST || args->car->cdr->type != LST) return;
		a = eval_thing(args->car->car, variables);
		if (a->type == NIL) continue;

		b = eval_thing(args->car->cdr->car, variables);
		copy_thing(r, b);
		free_thing(b);
		break;
	}
}

void operator_define(struct thing * r, struct thing * args, struct variable * variables)
{
	struct variable * v;
	struct thing * thing;
	if (args->type != LST || args->car->type != SYM ||args->cdr->type != LST)
		return;
	
	thing = malloc(sizeof(struct thing));
	copy_thing(thing, args->cdr->car);
	
	for (v = variables; v && v->next && strcmp(v->next->label, args->car->label); v = v->next);
	if (v->next)
	{
		free_thing(v->next->thing);
		v->next->thing = thing;
	} else
	{
		v->next = malloc(sizeof(struct variable));
		v->next->next = NULL;
		v->next->label = malloc(sizeof(char) * (strlen(args->car->label) + 1));
		strcpy(v->next->label, args->car->label);
		v->next->thing = thing;
	}
	
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
			v->next = NULL;
			v->label = malloc(sizeof(char) * (strlen(args->car->label) + 1));
			strcpy(v->label, args->car->label);
		} else
		{
			fprintf(stderr, "ERROR not a variable name!\n");
			exit(EXIT_FAILURE);			
		}
	}
	
	v = r->function->variables;
	r->function->variables = v->next;
	free(v->label);
	free(v);
}