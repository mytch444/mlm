#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "mlm.h"
#include "operators.c"
#include "script_operators.c"

#define N_FUNCTIONS 17
struct operator operators[] = {
	{ "+", &operator_add },
	{ "-", &operator_sub },
	{ "*", &operator_mul },
	{ "/", &operator_div },
	{ "=", &operator_equal },
	{ ">", &operator_greater },
	{ "is", &operator_is },
	{ "car", &operator_car },
	{ "cdr", &operator_cdr },
	{ "cons", &operator_cons },
	{ "cond", &operator_cond },
	{ "def", &operator_define },
	{ "\\", &operator_lambda },
	
	{ "exec", &operator_exec },
	{ "read", &operator_read },
	{ "print", &operator_print },
	{ "error", &operator_error },
};

void die(char * mes)
{
	fprintf(stderr, "%s\n", mes);
	exit(EXIT_FAILURE);
}

int thing_equivalent(struct thing * a, struct thing * b)
{
	struct thing * x, * y;
	if (a->type != b->type) return 0;
	switch (a->type)
	{
	case CHR:
	case INT: return (int) a->value == (int) b->value;
	case FLT: return a->value == b->value;
	case SYM: return !strcmp(a->label, b->label);
	case LST: 
		for (x = a->car, y = b->car; 
			thing_equivalent(x, y);
			x = x->cdr, y = y->cdr);
		return x->type == NIL && y->type == NIL;
	case FNC: /* cannot be bothered */
		return 0;
	default: return 1;
	}
}

void free_thing(struct thing * thing)
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
		/*free_thing(thing->function->thing);
		struct variable *o, * v = thing->function->variables;
		while (v)
		{
			o = v;
			v = v->next;
			free(o->label);
			free(o);
		}
		free(thing->function); */
		break;
	}
	
	free(thing);
}

void copy_thing(struct thing * n, struct thing * o)
{
	n->type = o->type;
	switch (n->type)
	{
	case CHR:
	case FLT:
	case INT: n->value = o->value; break;
	case LST:
		n->car = malloc(sizeof(struct thing));
		n->cdr = malloc(sizeof(struct thing));
		copy_thing(n->car, o->car);
		copy_thing(n->cdr, o->cdr);
		break;
	case SYM:
		n->label = malloc(sizeof(char) * (strlen(o->label) + 1));
		strcpy(n->label, o->label);
		break;
	case FNC:
		n->function = malloc(sizeof(struct function));
		n->function->thing = malloc(sizeof(struct thing));
		copy_thing(n->function->thing, o->function->thing);
		
		struct variable * w, * v = o->function->variables;
		if (v)
			w = n->function->variables = malloc(sizeof(struct variable));
		else w = n->function->variables = NULL;
		for (; v; v = v->next)
		{
			w->label = malloc(sizeof(char) * (strlen(v->label) + 1));
			strcpy(w->label, v->label);
			w->thing = NULL;
			if (v->next)
				w->next = malloc(sizeof(struct variable));
			else w->next = NULL;
			w = w->next;
		}
		
		break;
	}
}

struct thing * eval_thing(struct thing * thing, struct variable * variables)
{
	int i;
	struct variable * v;
	struct thing * t, * r = malloc(sizeof(struct thing));
	r->type = NIL;
	if (thing->type == LST)
	{
		t = malloc(sizeof(struct thing));
		copy_thing(t, thing);
		do
		{
			thing = t->car;
			t->car = eval_thing(t->car, variables);
			free_thing(thing);
		} while (t->car->type == LST && t->car->car->type == SYM);

		if (t->car->type == SYM)
		{
			for (i = 0; i < N_FUNCTIONS; i++)
			{
				if (strcmp(t->car->label, operators[i].label) == 0)
				{
					operators[i].func(r, t->cdr, variables);
					free_thing(t);
					return r;
				}
			}
			
			printf("%s\n", t->car->label);
			die("symbol not defined!");
		} else if (t->car->type == FNC)
		{
			thing = t;
			for (v = t->car->function->variables; v; v = v->next)
			{
				t = t->cdr;
				if (v->thing) die("how?\n");
				if (t->type == LST) v->thing = eval_thing(t->car, variables);
				else die("ERROR not enough arguments for lambda function!");
				if (!v->next) break;
			}
			v->next = variables;
			
			copy_thing(r, thing->car->function->thing);
			
			t = eval_thing(r, thing->car->function->variables);
			
			v->next = NULL;
			free_thing(thing);
			return t;
		} else return t;
	} else if (thing->type == SYM)
		for (v = variables; v; v = v->next)
			if (strcmp(v->label, thing->label) == 0) 
				{ thing = v->thing; break; }

	copy_thing(r, thing);
	return r;
}

char * char_list_to_string(struct thing * thing)
{
	struct thing * t;
	int l = 1;
	for (t = thing; t->type == LST; t = t->cdr) l++;
	char * str = malloc(sizeof(char) * l);
	
	l = 0;
	for (t = thing; t->type == LST; t = t->cdr)
		str[l++] = (char) t->car->value;
	str[l] = '\0';	
	
	return str;
}

void print_thing(struct thing * thing)
{
	switch (thing->type) {
	case NIL: 
		printf("()");
		break;
	case INT: 
		printf("%i", (int) thing->value);
		break;
	case CHR: 
		printf("%c", (int) thing->value);
		break;
	case FLT:
		printf("%f", thing->value);
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

struct thing * parse_string(char * str)
{
	char *c, tmp;
	struct thing * t, * thing = malloc(sizeof(struct thing));
	thing->type = NIL;
	if (*str == '(')
	{ /* parse list */
		t = thing;
		c = ++str;
		while (*c)
		{
			for (str = c; IS_SPACE(*str); str++) c++;
			if (*str == ')') break;
			while (!check_section_end(*c)) c++;
			tmp = *c;
			*c = '\0';
			t->cdr = malloc(sizeof(struct thing));
			t = t->cdr;
			t->type = LST;
			t->car = parse_string(str);
			*c = tmp;
		}
		
		t->cdr = malloc(sizeof(struct thing));
		t->cdr->type = NIL;
		t = thing;
		thing = t->cdr;
		free(t);
		
	} else if (*str == '\'')
	{ /* parse char */
		thing->type = CHR;
		for (c = ++str; *c && *c != '\''; c++);
		*c = '\0';
		thing->value = (float) parse_char(str);
		/* TODO; work with escape codes and unicode? */
	} else if (*str == '\"')
	{ /* parse string */
		t = thing;
		for (c = ++str; *c && *c != '\"'; c++)
		{
			t->type = LST;
			t->car = malloc(sizeof(struct thing));
			t->car->type = CHR;
			t->car->value = (float) parse_char(c);
			t->cdr = malloc(sizeof(struct thing));
			t = t->cdr;
		}
		t->type = NIL;
	} else if (*str >= '0' && *str <= '9'
		|| *str == '.'
		|| (*str == '-' && strlen(str) > 1))
	{ /* parse number */
		thing->type = INT;
		for (c = str; *c; c++) if (*c == '.') thing->type = FLT;
		if (thing->type == INT) thing->value = atoi(str);
		else thing->value = atof(str);
	} else
	{ /* function? */
		thing->type = SYM;
		thing->label = malloc(sizeof(char) * (strlen(str) + 1));
		strcpy(thing->label, str);
	}
	
	return thing;
}

void parse_hash(char * line, struct variable * variables)
{
	char * s;
	int i;
	
	if (*(line+1) == '!') return;
	
	/* line is now just the first word. */
	for (s = line; *s && !IS_SPACE(*s); s++);
	*s = '\0';
	/* and s the rest not including space */
	for (s++; IS_SPACE(*s); s++);
	
	if (strcmp(line, "include") == 0)
	{ /* include definitions from another file. */
		struct thing * state;
		char path[256];
		int fd = -1;
		for (i = 0; i < N_PATHS; i++)
		{
			sprintf(path, "%s/%s", library_paths[i], s);
			if ((fd = open(path, O_RDONLY)) > 0) break;
		}
		if (fd < 0)
		{
			fprintf(stderr, "Could not find %s include file\n", s);
			die("FAILED");
		}
		
		state = malloc(sizeof(state));
		state = parse_file(fd, state, variables);
		
		close(fd);
	}
}

struct thing * parse_file(int fd, struct thing * state, struct variable * variables)
{
	int i, comment = 0;
	ssize_t count = 1;
	char *str, *c, tmp;
	char buf[512];
	struct thing * parsed;
	
	while (count)
	{
		c = str = buf;
		count = read(fd, c, sizeof(char));
		if (IS_SPACE(*c)) continue;
		else if (*c == ';' || *c == '#')
		{
			while (count && *c != '\n') 
				count = read(fd, ++c, sizeof(char));
			*c = '\0';
			if (*str == '#')
				parse_hash(++str, variables);
			continue;
		}
		
		while (count && !check_section_end(*c))
			count = read(fd, ++c, sizeof(char));
		*c = '\0';
		
		while (*str && IS_SPACE(*str)) str++;
		if (!*str) continue;
		
		free_thing(state);
		parsed = parse_string(str);
		state = eval_thing(parsed, variables);
		printf("eval: ");
		print_thing(state);
		printf("\n");
		free_thing(parsed);
	}
	
	return state;
}

int main(int argc, char *argv[])
{
	int i, fd = 0, print = 1;
	struct thing * state;
	struct variable * variables;
	state = malloc(sizeof(struct thing));
	state->type = NIL;

	variables = malloc(sizeof(struct variable));
	variables->label = "";
	variables->next = NULL;
	
	for (i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-") == 0) break;
		else if (strcmp(argv[i], "-c") == 0) print = 0;
		else if (!fd) fd = open(argv[i], O_RDONLY);
	}

	if (fd < 0) die("ERROR opening file");
	state = parse_file(fd, state, variables);
	close(fd);
	
	if (!print) return EXIT_SUCCESS;
	print_thing(state);
	printf("\n");
	return EXIT_SUCCESS;
}
