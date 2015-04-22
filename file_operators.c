int operator_exec(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols)
{
	return 0;
}

int operator_read(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols)
{
	r->type = CHR;
	read(find_symbol(symbols, "stdin")->value, &r->value, 1);
	return 0;
}

int operator_write(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols)
{
	if (args->type != LST) err(1, "write bad arguments.");
	struct mlm_thing * t = eval_thing(args->car, symbols);
	
	char c = t->value;
	write(find_symbol(symbols, "stdout")->value, &c, 1);	
	return 0;
}

int operator_open(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols)
{
	if (args->type != LST || args->cdr->type != LST)
		err(1, "open bad arguments.");
	struct mlm_thing * f, * o, * c;
	char path[512];
	int i;
	
	f = eval_thing(args->car, symbols);
	o = eval_thing(args->cdr->car, symbols);
	
	for (i = 0, c = f; c->type == LST; c = c->cdr)
		path[i++] = c->car->value;
	path[i] = '\0';
	r->type = INT;
	r->value = open(path, o->value);
	
	free_thing(f);
	free_thing(o);
	return 0;
}

int operator_stdin(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols)
{
	r->type = INT;
	r->value = 0;
	return 0;
}

int operator_stdout(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols)
{
	r->type = INT;
	r->value = 1;
	return 0;
}
