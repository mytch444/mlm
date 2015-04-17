#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "mlm.h"
#include "operators.c"

#define N_FUNCTIONS 11
struct operator operators[N_FUNCTIONS] = {
	{ "+", &operator_add },
	{ "-", &operator_sub },
	{ "=", &operator_equal },
	{ ">", &operator_greater },
	{ "is", &operator_is },
	{ "car", &operator_car },
	{ "cdr", &operator_cdr },
	{ "cons", &operator_cons },
	{ "cond", &operator_cond },
	{ "def", &operator_define },
	{ "\\", &operator_lambda },
};

int thing_equivalent(struct thing * a, struct thing * b)
{
	struct thing * x, * y;
	if (a->type != b->type) return 0;
	switch (a->type)
	{
	case CHR:
	case INT: return a->value == b->value;
	case DBL: return a->point == b->point;
	case SYM: return !strcmp(a->label, b->label);
	case LST: 
		for (x = a->car, y = b->car; 
			thing_equivalent(x, y);
			x = x->cdr, y = y->cdr);
		return x->type == NIL && y->type == NIL;
	default: return 1;
	}
}

void free_thing(struct thing * thing)
{
	if (thing->type == LST)
	{	
		free_thing(thing->car);
		free_thing(thing->cdr);
	} else if (thing->type == SYM)
		free(thing->label);
	
	free(thing);
}

void copy_thing(struct thing * n, struct thing * o)
{
	n->type = o->type;
	switch (n->type)
	{
	case CHR:
	case INT: n->value = o->value; break;
	case DBL: n->point = o->point; break;
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
		do
			thing->car = eval_thing(thing->car, variables);
		while (thing->car->type == LST && thing->car->car->type == SYM);

		if (thing->car->type == SYM)
		{
			for (i = 0; i < N_FUNCTIONS; i++)
			{
				if (strcmp(thing->car->label, operators[i].label) == 0)
				{
					operators[i].func(r, thing->cdr, variables);
					return r;
				}
			}
			
			fprintf(stderr, "%s undefined.\n", thing->car->label);
			exit(EXIT_FAILURE);
		} else if (thing->car->type == FNC)
		{
			t = thing;
			for (v = t->car->function->variables; v; v = v->next)
			{
				t = t->cdr;
				if (t->type != LST) break;
				v->thing = t->car;
				if (!v->next) break;
			}
			v->next = variables;
			
			thing = eval_thing(thing->car->function->thing, thing->car->function->variables);
			v->next = NULL;
		}
	} else if (thing->type == SYM)
	{
		for (v = variables; v; v = v->next)
			if (strcmp(v->label, thing->label) == 0) { thing = v->thing; break; }
	}

givecopy:
	copy_thing(r, thing);
	
	return r;
}

void print_thing(struct thing * thing)
{
	switch (thing->type)
	{
	case NIL: 
		printf("()");
		break;
	case INT: 
		printf("%i", thing->value);
		break;
	case CHR: 
		printf("%c", thing->value);
		break;
	case DBL:
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
	}
}

char * forward_section(char * str)
{
	int b = 0;
	if (*str == '(')
	{
		for (; *str && !(!b && *(str-1) == ')'); str++)
		{
			if (*str == '(') b++;
			if (*str == ')') b--;
		}
	} else for (; *str && *str != ')' && !IS_SPACE(*str); str++);
	return str;
}

struct thing * parse_string(char * str)
{
	char *c, tmp;
	struct thing * root, * thing = malloc(sizeof(struct thing));
	if (*str == '(')
	{ /* parse list */
		c = ++str;
		root = thing;
		while (*c && *c != ')')
		{
			for (str = c; IS_SPACE(*str); str++);
			if (*str == ')') break;
			c = forward_section(str);
			tmp = *c;
			*c = '\0';
			thing->cdr = malloc(sizeof(struct thing));
			thing = thing->cdr;
			thing->type = LST;
			thing->car = parse_string(str);
			*c = tmp;
		}
		
		thing->cdr = malloc(sizeof(struct thing));
		thing->cdr->type = NIL;
		thing = root->cdr;
		/* don't want it to free everthing inside root */
		free(root);
		
	} else if (*str == '\'')
	{ /* parse char */
		thing->type = CHR;
		for (c = ++str; *c && *c != '\''; c++);
		*c = '\0';
		thing->value = *str;
		/* TODO; work with escape codes and unicode? */
	} else if (*str >= '0' && *str <= '9'
		|| *str == '.'
		|| (*str == '-' && strlen(str) > 1))
	{ /* parse number */
		thing->type = INT;
		for (c = str; *c; c++) if (*c == '.') thing->type = DBL;
		if (thing->type == INT) thing->value = atoi(str);
		else thing->point = atof(str);
	} else
	{ /* function? */
		thing->type = SYM;
		thing->label = malloc(sizeof(char) * (strlen(str) + 1));
		strcpy(thing->label, str);
	}
	
	return thing;
}

int main(int argc, char *argv[])
{
	int i;
	char *str, *c, tmp;
	char buf[512];
	ssize_t count;
	struct thing * parsed, * state;
	struct variable * variables;
	state = malloc(sizeof(struct thing));
	state->type = NIL;

	variables = malloc(sizeof(struct variable));
	variables->label = "mol";
	variables->thing = malloc(sizeof(struct thing));
	variables->thing->type = INT;
	variables->thing->value = 42;
	variables->next = NULL;
	
	count = read(0, buf, sizeof(buf));
	if (count == -1)
	{
		printf("read error: %i\n", errno);
		return EXIT_FAILURE;
	}
	
	str = c = buf;
	while (1)
	{
		while (*str && IS_SPACE(*str)) str++;
		c = forward_section(str);
		if (c - buf >= count) break;
		tmp = *c;
		*c = '\0';
		free_thing(state);
		parsed = parse_string(str);
		state = eval_thing(parsed, variables);
		free_thing(parsed);
		*c = tmp;
		str = c;
	}
	
	print_thing(state);
	printf("\n");
	return EXIT_SUCCESS;
}
