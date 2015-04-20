#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "mlm.h"
#include "operators.c"
#include "file_operators.c"

void die(char * mes)
{
	fprintf(stderr, "%s\n", mes);
	exit(EXIT_FAILURE);
}

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
	default: return 1;
	}
}

void free_thing(struct mlm_thing * thing)
{
	switch (thing->type)
	{
	case LST:
		if (thing->car)
			free_thing(thing->car);
		if (thing->cdr)
			free_thing(thing->cdr);
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
			if (o->name) free(o->name);
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
		s->name = NULL; s->thing = NULL;
		s->next = NULL;

		for (t = o->function->symbols->next; t; t = t->next)
		{
			s->next = malloc(sizeof(struct mlm_symbol));
			s = s->next;
			s->name = malloc(sizeof(char) * (strlen(t->name) + 1));
			strcpy(s->name, t->name);
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

struct mlm_thing * eval_thing(struct mlm_thing * thing, struct mlm_symbol * symbols)
{
	int i;
	struct mlm_symbol * s;
	struct mlm_thing * t = thing, * r = malloc(sizeof(struct mlm_thing));
	r->type = NIL;
	if (thing->type == LST)
	{
		t = malloc(sizeof(struct mlm_thing));
		copy_thing(t, thing);
		do
		{
			thing = t->car;
			t->car = eval_thing(t->car, symbols);
			free_thing(thing);
		} while (t->car->type == LST && t->car->car->type == SYM);

		if (t->type != LST || t->car->type != FNC) return t;
		if (!t->car->function->thing)
		{
			t->car->function->func(r, t->cdr, symbols);
			free_thing(t);
			return r;
		} else
		{
			thing = t;
			for (s = t->car->function->symbols; s && s->next; s = s->next)
			{
				if (t->cdr->type == LST)
					s->next->thing = eval_thing((t = t->cdr)->car, symbols);
				else
					s->next->thing = eval_thing(t->car, symbols);
			}
			s->next = symbols->next;
			
			t = eval_thing(thing->car->function->thing,
			               thing->car->function->symbols);
			s->next = NULL;
			free_thing(thing);
			return t;
		}
	} else if (thing->type == SYM)
		t = find_symbol(symbols, thing->label);

	if (t) copy_thing(r, t);
	else copy_thing(r, thing);
	return r;
}

char * char_list_to_string(struct mlm_thing * thing)
{
	struct mlm_thing * t;
	int l = 1;
	for (t = thing; t->type == LST; t = t->cdr) l++;
	char * str = malloc(sizeof(char) * l);
	
	l = 0;
	for (t = thing; t->type == LST; t = t->cdr)
		str[l++] = (char) t->car->value;
	str[l] = '\0';	
	
	return str;
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
		printf("lambda");
		break;
	}
}

int check_section_end(char c)
{
	static int b = 0, q = 0, qq = 0;
	if (!c || (IS_SPACE(c) || c == ')' || c == ';')
                  && b <= 0 && !q && !qq)
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

struct mlm_thing * parse_string(char * str)
{
	char *c, tmp;
	struct mlm_thing * t, * thing;
	t = thing = malloc(sizeof(struct mlm_thing));
	thing->type = NIL;
	
	if (*str == '(')
	{ /* parse list */
		c = ++str;
		while (*c)
		{
			for (str = c; *str && IS_SPACE(*str); str++) c++;
			if (*str == ')') break;
			if (!*str) die("UNFINISHED LIST!");
			while (!check_section_end(*c)) c++;
			*(c++) = '\0';
			t->type = LST;
			t->car = parse_string(str);
			t->cdr = malloc(sizeof(struct mlm_thing));
			t = t->cdr;
		}
		
		t->type = NIL;
		
	} else if (*str == '\'')
	{ /* parse char */
		t->type = CHR;
		for (c = ++str; *c && *c != '\''; c++);
		*c = '\0';
		t->value = (float) parse_char(str);
	} else if (*str == '\"')
	{ /* parse string */
		for (c = ++str; *c && *c != '\"'; c++)
		{
			t->type = LST;
			t->car = malloc(sizeof(struct mlm_thing));
			t->car->type = CHR;
			t->car->value = (float) parse_char(c);
			t->cdr = malloc(sizeof(struct mlm_thing));
			t = t->cdr;
		}
		t->type = NIL;
	} else if (*str >= '0' && *str <= '9'
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

int include_file(char * name, struct mlm_path * paths, struct mlm_symbol * symbols)
{
	struct mlm_thing * state;
	char path[256];
	struct mlm_path * p;
	int fd = -1;
	
	if (*name == '/') fd = open(name, O_RDONLY);
	for (p = paths; fd < 0 && p; p = p->next)
	{
		sprintf(path, "%s/%s", p->path, name);
		fd = open(path, O_RDONLY);
	}
	
	if (fd > 0)
	{
		state = parse_file(fd, paths, symbols);
		free_thing(state);
		close(fd);
		return 0;
	}

	fprintf(stderr, "Could not find file '%s' to include\n", name);
	return 1;
}

struct mlm_thing * parse_file(int fd, struct mlm_path * paths, struct mlm_symbol * symbols)
{
	int i, comment = 0;
	ssize_t count = 1;
	char *str, *c, tmp;
	char buf[512];
	struct mlm_thing * parsed, * state = NULL;
	
	while (count)
	{
		if (!fd)
		{
			printf("> "); fflush(stdout);
		}
		
		c = str = buf;
		count = read(fd, c, sizeof(char));
		if (IS_SPACE(*c)) continue;
		else if (*c == ';' || *c == '#')
		{
			while (count && *c != '\n') 
				count = read(fd, ++c, sizeof(char));
			*c = '\0';
			if (*str == '#' && *(++str) != '!')
				include_file(str, paths, symbols);
			continue;
		}
		
		while (count && !check_section_end(*c))
			count = read(fd, ++c, sizeof(char));
		*c = '\0';
		
		while (*str && IS_SPACE(*str)) str++;
		if (!*str) continue;
		
		if (state)
			free_thing(state);
		parsed = parse_string(str);
		state = eval_thing(parsed, symbols);
		if (!fd)
		{
			print_thing(state);
			printf("\n");
		}
		free_thing(parsed);
	}
	
	return state;
}

#define ADD_NEW_OPERATOR(s, L, F) \
                        s->next = malloc(sizeof(struct mlm_symbol)); \
                        s = s->next; \
                        s->name = L; \
                        s->thing = F; \
                        s->next = NULL;

void add_default_symbols(struct mlm_symbol * s)
{
	for (s; s && s->next; s = s->next);
	ADD_NEW_OPERATOR(s, "+", &operator_add_thing);
	ADD_NEW_OPERATOR(s, "-", &operator_sub_thing);
	ADD_NEW_OPERATOR(s, "*", &operator_mul_thing);
	ADD_NEW_OPERATOR(s, "/", &operator_div_thing);
	ADD_NEW_OPERATOR(s, "|", &operator_or_thing);
	ADD_NEW_OPERATOR(s, "&", &operator_and_thing);
	ADD_NEW_OPERATOR(s, "^", &operator_xor_thing);
	ADD_NEW_OPERATOR(s, "=", &operator_equal_thing);
	ADD_NEW_OPERATOR(s, ">", &operator_greater_thing);
	ADD_NEW_OPERATOR(s, "is", &operator_is_thing);
	ADD_NEW_OPERATOR(s, "car", &operator_car_thing);
	ADD_NEW_OPERATOR(s, "cdr", &operator_cdr_thing);
	ADD_NEW_OPERATOR(s, "cons", &operator_cons_thing);
	ADD_NEW_OPERATOR(s, "cond", &operator_cond_thing);
	ADD_NEW_OPERATOR(s, "def", &operator_def_thing);
	ADD_NEW_OPERATOR(s, "\\", &operator_lambda_thing);
	ADD_NEW_OPERATOR(s, "exec", &operator_exec_thing);
	ADD_NEW_OPERATOR(s, "open", &operator_open_thing);
	ADD_NEW_OPERATOR(s, "print", &operator_print_thing);
	ADD_NEW_OPERATOR(s, "read", &operator_read_thing);
}

int main(int argc, char *argv[])
{
	int i, fd = 0;
	struct mlm_thing * state;
	struct mlm_symbol * symbols;
	struct mlm_path * paths, * p;
	
	state = malloc(sizeof(struct mlm_thing));
	state->type = NIL;

	paths = p = malloc(sizeof(struct mlm_path));
	p->path = ".";
	p->next = NULL;
	
	symbols = malloc(sizeof(struct mlm_symbol));
	symbols->name = NULL;
	symbols->next = NULL;
	
	add_default_symbols(symbols);
	
	for (i = 1; i < argc; i++)
	{
		if (*argv[i] == '-')
		{
			if (*(++argv[i]) == 'L')
			{
				printf("adding library path\n");
				p->next = malloc(sizeof(struct mlm_path));
				p = p->next;
				p->path = ++argv[i];
				p->next = NULL;
			} else break;
		} else if (!fd) fd = open(argv[i], O_RDONLY);
	}

	p->next = malloc(sizeof(struct mlm_path));
	p->next->path = STD_LIBRARY_PATH;
	p->next->next = NULL;

	if (fd < 0) fd = 0;
	state = parse_file(fd, paths, symbols);
	close(fd);
	
	if (fd > 0)
	{
		print_thing(state);
		printf("\n");
	}
	return EXIT_SUCCESS;
}
