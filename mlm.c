#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "mlm.h"
#include "operators.c"

#define N_FUNCTIONS 13
struct operator operators[N_FUNCTIONS] = {
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
	default: return 1;
	}
}

void free_thing(struct thing * thing)
{
	switch (thing->type)
	{
	case LST:
		free_thing(thing->car);
		free_thing(thing->cdr);
		break;
	case SYM:
		free(thing->label);
		break;
	case FNC:
/*		free_thing(thing->function->thing);
		struct variable *o, * v = thing->function->variables;
		while (v)
		{
			o = v;
			v = v->next;
			free(o->label);
			free(o);
		}
		free(thing->function);
			*/
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
			printf("copied var %s\n", w->label);
			w->thing = NULL;
			if (v->next)
				w->next = malloc(sizeof(struct variable));
			else w->next = NULL;
			w = w->next;
		}
		
		for (v = n->function->variables; v; v = v->next)
			printf("have copied %s\n", v->label);

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
			
			die("symbol not defined!");
		} else if (t->car->type == FNC)
		{
			printf("evaling func\n");
			thing = t;
			for (v = t->car->function->variables; v; v = v->next)
			{
				t = t->cdr;
				printf("setting %s to: \n    ", v->label);
				if (v->thing) die("how?\n");
				if (t->type == LST) v->thing = eval_thing(t->car, variables);
				else die("ERROR not enough arguments for lambda function!");
				print_thing(v->thing);
				printf("\n");
				if (!v->next) break;
			}
			v->next = variables;
			
			copy_thing(r, thing->car->function->thing);
			
			t = eval_thing(r, thing->car->function->variables);
			
			v->next = NULL;
			free_thing(thing);
			return t;
		}
	} else if (thing->type == SYM)
	{
		printf("tying to sub %s\n", thing->label);
		for (v = variables; v; v = v->next)
			if (strcmp(v->label, thing->label) == 0) { printf("found match\n"); thing = v->thing; break; }
	}

givecopy:
	copy_thing(r, thing);
	
	return r;
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
		/* don't free everthing inside root */
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
	variables->label = "";
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
		printf("EVAL: ");
		print_thing(state);
		printf("\n");
		free_thing(parsed);
		
		*c = tmp;
		str = c;
	}
	
	print_thing(state);
	printf("\n");
	return EXIT_SUCCESS;
}
