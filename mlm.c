#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "mlm.h"
#include "operators.c"
#include "file_operators.c"

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

struct mlm_thing * eval_thing(struct mlm_thing * thing, struct mlm_symbol * symbols)
{
	struct mlm_thing * t = NULL, * r = malloc(sizeof(struct mlm_thing));
	r->type = NIL;
	if (thing->type == LST)
	{
		t = malloc(sizeof(struct mlm_thing));
		copy_thing(t, thing);
		do
		{
			thing = t->car;
			t->car = eval_thing(thing, symbols);
			free_thing(thing);
		} while (t->car->type == LST
		     && (t->car->car->type == SYM || t->car->car->type == FNC));
		
		if (t->car->type != FNC) return t;
		
		int e = 0;
		thing = t;
		
		if (!t->car->function->thing)
			e = t->car->function->func(r, t->cdr, symbols);
		else
		{
			struct mlm_symbol * s;
			for (s = t->car->function->symbols; !e && s && s->next; s = s->next)
			{
				if (t->cdr->type == LST)
					s->next->thing =
						eval_thing((t = t->cdr)->car, symbols);
				else e = 1;
			}
			if (!e)
			{
				s->next = symbols->next;
				free_thing(r);
				r = eval_thing(thing->car->function->thing,
			                       thing->car->function->symbols);
				s->next = NULL;
			}
		}
		
		if (e)
		{
			fprintf(stderr, "Error while evaluating: ");
			print_thing(thing);
			fprintf(stderr, "\n");
			r->type = NIL;
		}
		
		free_thing(thing);
		
		return r;
	} else if (thing->type == SYM)
		t = find_symbol(symbols, thing->label);
	
	if (t) copy_thing(r, t);
	else copy_thing(r, thing);
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
		printf("func");
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
	}
	t->type = NIL;
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
		state = parse_file(fd, 0, paths, symbols);
		free_thing(state);
		close(fd);
		return 0;
	}

	fprintf(stderr, "Could not find file '%s' to include\n", name);
	return 1;
}

struct mlm_thing * parse_file(int fd, int inter, struct mlm_path * paths, struct mlm_symbol * symbols)
{
	ssize_t count = 1;
	char *str, *c, buf[512];
	struct mlm_thing * parsed, * state = NULL;
	
	while (count)
	{
		if (inter)
		{
			printf("> "); fflush(stdout);
		}
		
		c = str = buf;
		count = read(fd, c, sizeof(char));
		if (!count || IS_SPACE(*c)) continue;
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
		
		if (!count && check_section_end(*c))
		{
			fprintf(stderr, "Reached end of input without ending section!\n");
			return NULL;
		}
		
		*c = '\0';
		
		if (state)
			free_thing(state);
		parsed = parse_string(str);
		state = eval_thing(parsed, symbols);
		free_thing(parsed);
		
		if (inter)
		{
			print_thing(state);
			printf("\n");
		}
	}
	
	return state;
}

void add_operator_next(struct mlm_symbol * s,
                  char * name,
                  int (*func)(struct mlm_thing *,
                              struct mlm_thing *,
                              struct mlm_symbol *))
{
	for (; s && s->next; s = s->next);
	s->next = malloc(sizeof(struct mlm_symbol));
	s = s->next;
	s->name = name;
	s->next = NULL;
	s->thing = malloc(sizeof(struct mlm_thing));
	s->thing->type = FNC;
	s->thing->function = malloc(sizeof(struct mlm_function));
	s->thing->function->symbols = malloc(sizeof(struct mlm_symbol));
	s->thing->function->symbols->name = NULL;
	s->thing->function->symbols->next = NULL;
	s->thing->function->thing = NULL;
	s->thing->function->func = func;
}

struct mlm_symbol * make_default_symbols()
{
	struct mlm_symbol * s, * symbols;
	s = symbols = malloc(sizeof(struct mlm_symbol));
	add_operator_next(s, "+", &operator_add);
	add_operator_next(s, "-", &operator_sub);
	add_operator_next(s, "*", &operator_mul);
	add_operator_next(s, "/", &operator_div);
	add_operator_next(s, "|", &operator_or);
	add_operator_next(s, "&", &operator_and);
	add_operator_next(s, "^", &operator_xor);
	add_operator_next(s, "=", &operator_equal);
	add_operator_next(s, ">", &operator_greater);
	add_operator_next(s, "is", &operator_is);
	add_operator_next(s, "car", &operator_car);
	add_operator_next(s, "cdr", &operator_cdr);
	add_operator_next(s, "cons", &operator_cons);
	add_operator_next(s, "cond", &operator_cond);
	add_operator_next(s, "def", &operator_def);
	add_operator_next(s, "\\", &operator_lambda);
	add_operator_next(s, "exec", &operator_exec);
	add_operator_next(s, "write", &operator_write);
	add_operator_next(s, "read", &operator_read);
	add_operator_next(s, "open", &operator_open);
	
	for (s = symbols->next; s; s = s->next);
	return symbols;
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
	
	symbols = make_default_symbols();
	
	for (i = 1; i < argc; i++)
	{
		if (*argv[i] == '-')
		{
			++argv[i];
			if (*argv[i] == 'L')
			{
				printf("adding library path\n");
				p->next = malloc(sizeof(struct mlm_path));
				p = p->next;
				p->path = ++argv[i];
				p->next = NULL;
			} else if (*argv[i] == 'l')
			{
				include_file(++argv[i], paths, symbols);
			} else
			{
				fprintf(stderr, "Bad option -%s\n", argv[i]);
			}
		} else if (!fd) fd = open(argv[i], O_RDONLY);
	}

	p->next = malloc(sizeof(struct mlm_path));
	p->next->path = STD_LIBRARY_PATH;
	p->next->next = NULL;

	if (fd < 0) fd = 0;
	state = parse_file(fd, 1, paths, symbols);
	close(fd);
	
	if (fd > 0)
	{
		print_thing(state);
		printf("\n");
	}
	return EXIT_SUCCESS;
}
