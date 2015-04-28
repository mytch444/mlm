#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "mlm.h"
#include "operators.c"

int thing_equivalent(struct mlm_thing * a, struct mlm_thing * b)
{
	struct mlm_thing * x, * y;
	if (a->type != b->type) return 0;
	switch (a->type)
	{
	case CHR:
	case INT: return a->value == b->value;
	case FLT: return a->point == b->point;
	case LST: 
		for (x = a->car, y = b->car; 
			thing_equivalent(x, y);
			x = x->cdr, y = y->cdr);
		return x->type == NIL && y->type == NIL;
	case FNC: 
		if (a->function->thing && b->function->thing)
			return thing_equivalent(a->function->thing,
			                        b->function->thing);
		else if (!a->function->thing && !b->function->thing)
			return a->function->func == b->function->func;
		else return 0;
	case SYM: return strcmp(a->label, b->label);
	case NIL: return 1;
	}
	return -1;
}

void free_thing(struct mlm_thing * thing)
{
	switch (thing->type)
	{
	case NIL:
	case CHR:
	case INT:
	case FLT: break;
	case LST:
		if (thing->car) free_thing(thing->car);
		if (thing->cdr) free_thing(thing->cdr);
		break;
	case SYM:
		free(thing->label);
		break;
	case FNC:
		if (thing->function->thing)
			free_thing(thing->function->thing);
		struct mlm_symbol *o, * v = thing->function->symbols;
		while (v)
		{
			o = v;
			v = v->next;
			if (o->thing) free_thing(o->thing);
			free(o);
		}
		free(thing->function);
		break;
	}
	free(thing);
}

void copy_thing(struct mlm_thing * n, struct mlm_thing * o)
{
	n->type = o->type;
	switch (n->type)
	{
	case NIL: break;
	case CHR:
	case INT: n->value = o->value; break;
	case FLT: n->point = o->point; break;
	case LST:
		n->car = malloc(sizeof(struct mlm_thing));
		n->cdr = malloc(sizeof(struct mlm_thing));
		copy_thing(n->car, o->car);
		copy_thing(n->cdr, o->cdr);
		break;
	case SYM:
		n->label = malloc(sizeof(char) * (strlen(o->label) + 1));
		strcpy(n->label, o->label);
		break;
	case FNC:
		n->function = malloc(sizeof(struct mlm_function));
		if (o->function->thing)
		{
			n->function->thing = malloc(sizeof(struct mlm_thing));
			copy_thing(n->function->thing, o->function->thing);
		} else
		{
			n->function->func = o->function->func;
			n->function->thing = NULL;
		}
		
		struct mlm_symbol * s, * t;
		s = n->function->symbols = malloc(sizeof(struct mlm_symbol));
		s->name = NULL; s->thing = NULL; s->next = NULL;

		for (t = o->function->symbols->next; t; t = t->next)
		{
			s->next = malloc(sizeof(struct mlm_symbol));
			s = s->next;
			s->name = t->name;
			s->thing = NULL;
			s->next = NULL;
		}
		
		break;
	}
}

struct mlm_thing * find_symbol(struct mlm_symbol * s, char * name)
{
	for (s = s->next; s; s = s->next)
		if (strcmp(s->name, name) == 0) 
			return s->thing;
	return NULL;
}

struct mlm_thing * eval_function(struct mlm_function * f,
                                 struct mlm_thing * args,
                                 struct mlm_symbol * symbols)
{
	struct mlm_thing * r, * t;
	int e = 0;
	
	if (!f->thing)
	{
		r = malloc(sizeof(struct mlm_thing));
		r->type = NIL;
		if (f->func(r, args, symbols))
			goto error;
	} else
	{
		struct mlm_symbol * s;
		for (s = f->symbols;
			!e && s && s->next; s = s->next)
		{
			if (args->type == LST)
			{
				s->next->thing = eval_thing(args->car,
				                            symbols);
				args = args->cdr;
			} else goto error;
		}
		
		s->next = symbols->next;
		for (t = f->thing; t->type == LST; t = t->cdr)
			r = eval_thing(t->car, f->symbols);
		s->next = NULL;
	}
	
	return r;
error:
	fprintf(stderr, "Error while evaluating function!\n");
	r = malloc(sizeof(struct mlm_thing));
	r->type = NIL;
	return r;
}

struct mlm_thing * eval_thing(struct mlm_thing * thing,
			      struct mlm_symbol * symbols)
{
	struct mlm_thing * t, * r;
	
	if (thing->type == SYM)
		t = find_symbol(symbols, thing->label);
	else t = NULL;
	
	if (thing->type != LST)
	{
		r = malloc(sizeof(struct mlm_thing));
		if (t) copy_thing(r, t);
		else copy_thing(r, thing);
		return r;
	}
	
	t = malloc(sizeof(struct mlm_thing));
	t->type = LST;
	t->car = eval_thing(thing->car, symbols);
	if (t->car->type == FNC)
	{
		r = eval_function(t->car->function, thing->cdr, symbols);
		free_thing(t->car);
		free(t);
	} else
	{
		r = t;
		for (thing = thing->cdr; 
		     thing->type == LST;
		     thing = thing->cdr, t = t->cdr)
		{
			t->cdr = malloc(sizeof(struct mlm_thing));
			t->cdr->type = LST;
			t->cdr->car = eval_thing(thing->car, symbols);
		}
		t->cdr = malloc(sizeof(struct mlm_thing));
		t->cdr->type = NIL;
	}
	return r;
}

void print_thing(struct mlm_thing * thing)
{
	switch (thing->type) {
	case NIL: 
		printf("()");
		break;
	case INT: 
		printf("%i", thing->value);
		break;
	case CHR: 
		printf("%c", thing->value);
		break;
	case FLT:
		printf("%f", thing->point);
		break;
	case LST:
		printf("(");
		print_thing(thing->car);
		printf(" ");
		print_thing(thing->cdr);
		printf(")");
		break;
	case SYM:
		printf("%s", thing->label);
		break;
	case FNC:
		printf("func( ");
		struct mlm_symbol *s = thing->function->symbols;
		for (s = s->next; s; s = s->next) printf("%s ", s->name);
		printf(")");
		break;
	default: printf("what is this?\n");
	}
}

int check_section_end(char c)
{
	static int b = 0, q = 0, qq = 0;
	if (!c || ((IS_SPACE(c) || c == ')' || c == ';')
                    && b <= 0 && !q && !qq))
	{
		b = q = qq = 0;
		return 1;
	}

	switch (c)
	{
	case '(': if (!q && !qq) b++; break;
	case ')': if (!q && !qq) b--; break;
	case '\'': if (!qq) q = !q; break;
	case '\"': qq = !qq; break;
	}
	
	return 0;
}

char parse_char(char * cc)
{
	if (*cc == '\\')
	{
		switch (*(++cc))
		{
		case 'n': return '\n';
		case 'r': return '\r';
		case '\'': return '\'';
		case '\"': return '\"';
		case '0': return '\0';
		case 'v': return '\v';
		case '\\': return '\\';
		default: return 0;
		}
	} else return *cc;
}

void parse_char_list(struct mlm_thing * t, char * str)
{
	char *c;
	for (c = str; *c && *c != '\"'; c++)
	{
		t->type = LST;
		t->car = malloc(sizeof(struct mlm_thing));
		t->car->type = CHR;
		t->car->value = parse_char(c);
		t->cdr = malloc(sizeof(struct mlm_thing));
		t = t->cdr;
		t->type = NIL;
	}
}

struct mlm_thing * parse_string(char * str)
{
	char *c;
	struct mlm_thing * t, * thing;
	t = thing = malloc(sizeof(struct mlm_thing));
	thing->type = NIL;
	
	if (*str == '(')
	{ /* parse list */
		c = ++str;
		while (*c)
		{
			for (str = c; *str && IS_SPACE(*str); str++) c++;
			if (*str == ')') break; /* nil */
			while (!check_section_end(*c)) c++;
			*(c++) = '\0';
			t->type = LST;
			t->car = parse_string(str);
			t->cdr = malloc(sizeof(struct mlm_thing));
			t = t->cdr;
			t->type = NIL;
		}
	} else if (*str == '\'')
	{ /* parse char */
		t->type = CHR;
		for (c = ++str; *c && *c != '\''; c++);
		*c = '\0';
		t->value = parse_char(str);
	} else if (*str == '\"')
	{ /* parse string */
		parse_char_list(thing, ++str);
	} else if ((*str >= '0' && *str <= '9')
	            || *str == '.'
	            || (*str == '-' && strlen(str) > 1))
	{ /* parse number */
		t->type = INT;
		for (c = str; *c && t->type == INT; c++)
			if (*c == '.') t->type = FLT;
		if (t->type == INT) t->value = atoi(str);
		else t->value = atof(str);
	} else
	{ /* symbol */
		t->type = SYM;
		t->label = malloc(sizeof(char) * (strlen(str) + 1));
		strcpy(t->label, str);
	}
	
	return thing;
}

struct mlm_thing * parse_file(int fd,
	                      struct mlm_symbol * symbols)
{
	ssize_t count = 1;
	char *str, *c, buf[512];
	struct mlm_thing * parsed, * state = NULL;
	
	while (count)
	{
		if (!fd)
		{
			printf("> ");
			fflush(stdout);
		}
		
		c = str = buf;
		count = read(fd, c, sizeof(char));
		if (!count || IS_SPACE(*c)) continue;
		else if (*c == '#')
		{
			while (count && *c != '\n') 
				count = read(fd, ++c, sizeof(char));
			continue;
		}
		
		while (count && !check_section_end(*c))
			count = read(fd, ++c, sizeof(char));
		
		if (!count && check_section_end(*c))
		{
			fprintf(stderr, 
				"Reached end of input without ending section!\n");
			return NULL;
		}
		
		*c = '\0';
		
		if (state)
			free_thing(state);
		parsed = parse_string(str);
		state = eval_thing(parsed, symbols);
		free_thing(parsed);
		
		if (!fd)
		{
			print_thing(state);
			printf("\n");
		}
	}
	
	return state;
}

void add_definition(struct mlm_symbol *s,
                    char * name,
                    struct mlm_thing * thing)
{
	for (; s && s->next && strcmp(s->next->name, name);
	       s = s->next);
	if (s->next) free_thing(s->next->thing);
	else
	{
		s->next = malloc(sizeof(struct mlm_symbol));
		s->next->next = NULL;
		s->next->name = malloc(sizeof(char) *
		                       (strlen(name) + 1));
		strcpy(s->next->name, name);
	}
	
	s->next->thing = thing;
}

struct mlm_thing * make_operator(
             int (*func)(struct mlm_thing *,
                         struct mlm_thing *,
                         struct mlm_symbol *))
{
	struct mlm_thing * t;
	t = malloc(sizeof(struct mlm_thing));
	t->type = FNC;
	t->function = malloc(sizeof(struct mlm_function));
	t->function->symbols = malloc(sizeof(struct mlm_symbol));
	t->function->symbols->name = NULL;
	t->function->symbols->next = NULL;
	t->function->thing = NULL;
	t->function->func = func;
	return t;
}

struct mlm_symbol * make_default_symbols()
{
	struct mlm_symbol * s
		= malloc(sizeof(struct mlm_symbol));

	add_definition(s, "+", make_operator(&operator_add));
	add_definition(s, "-", make_operator(&operator_sub));
	add_definition(s, "*", make_operator(&operator_mul));
	add_definition(s, "/", make_operator(&operator_div));
	add_definition(s, "|", make_operator(&operator_or));
	add_definition(s, "&", make_operator(&operator_and));
	add_definition(s, "^", make_operator(&operator_xor));
	add_definition(s, "=", make_operator(&operator_equal));
	add_definition(s, ">", make_operator(&operator_greater));
	add_definition(s, "equal-type", make_operator(&operator_equal_type));
	add_definition(s, "car", make_operator(&operator_car));
	add_definition(s, "cdr", make_operator(&operator_cdr));
	add_definition(s, "cons", make_operator(&operator_cons));
	add_definition(s, "cond", make_operator(&operator_cond));
	add_definition(s, "def", make_operator(&operator_def));
	add_definition(s, "\\", make_operator(&operator_lambda));
	add_definition(s, "do", make_operator(&operator_do));
	add_definition(s, "load", make_operator(&operator_load));
	add_definition(s, "exec", make_operator(&operator_exec));
	return s;
}

int main(int argc, char *argv[])
{
	int i, fd = 0;
	struct mlm_thing * state;
	struct mlm_symbol * symbols;
	
	state = malloc(sizeof(struct mlm_thing));
	state->type = NIL;

	symbols = make_default_symbols();
	
	for (i = 1; i < argc; i++)
		if (!fd) fd = open(argv[i], O_RDONLY);

	if (fd < 0) fd = 0;
	state = parse_file(fd, symbols);
	close(fd);

	return EXIT_SUCCESS;
}
