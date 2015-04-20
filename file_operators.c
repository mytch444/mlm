void operator_exec(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols)
{

}

MLM_NEW_FUNCTION_THING(operator_exec, operator_exec_function, operator_exec_thing)

void operator_read(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols)
{

}

MLM_NEW_FUNCTION_THING(operator_read, operator_read_function, operator_read_thing)

void operator_print(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols)
{
	if (args->type != LST || args->cdr->type != LST) die("Print has wrong arguments!");
	struct mlm_thing * f = eval_thing(args->car, symbols);
	struct mlm_thing * t = eval_thing(args->cdr->car, symbols);
	
	char c = (int) t->value;
	write((int) f->value, &c, 1);	
	r->type = INT;
	r->value = 0;
}

MLM_NEW_FUNCTION_THING(operator_print, operator_print_function, operator_print_thing)

void operator_open(struct mlm_thing * r, struct mlm_thing * args, struct mlm_symbol * symbols)
{
	if (args->type != LST) die("Open is wrong arguments!");
	struct mlm_thing * f = eval_thing(args->car, symbols);
	struct mlm_thing * o = eval_thing(args->cdr->car, symbols);
	char * file = char_list_to_string(f);
	r->type = INT;
	r->value = open(file, (int) f->value);
}

MLM_NEW_FUNCTION_THING(operator_open, operator_open_function, operator_open_thing)
