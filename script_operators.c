void operator_exec(struct thing * r, struct thing * args, struct variable * variables)
{

}

void operator_read(struct thing * r, struct thing * args, struct variable * variables)
{

}

void operator_print_to(FILE *f, struct thing * args, struct variable * variables)
{
	if (args->type != LST) die("ERROR print/error has no arguments!");
	struct thing * t = eval_thing(args->car, variables);
	char * str = char_list_to_string(t);
	free_thing(t);
	fprintf(f, "%s\n", str);}

void operator_print(struct thing * r, struct thing * args, struct variable * variables)
{
	operator_print_to(stdout, args, variables);
}

void operator_error(struct thing * r, struct thing * args, struct variable * variables)
{
	operator_print_to(stderr, args, variables);
}
