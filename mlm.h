#define NIL 0
#define INT 1
#define CHR 2
#define LST 3
#define SYM 4
#define DBL 5
#define FNC 6

#define IS_SPACE(X) (X == '\n' || X == '\t' || X == '\r' || X == ' ')

struct function
{
	struct thing * thing;
	struct variable * variables;
};

struct thing
{
	int type;
	int value;
	double point;
	char * label;
	struct thing * car;
	struct thing * cdr;
	struct function * function;
};

struct variable
{
	char * label;
	struct thing * thing;
	struct variable * next;
};

struct operator
{
	char * label;
	void (*func)(struct thing * result,
	             struct thing * args,
	             struct variable * variables);
};

int thing_equivalent(struct thing * a, struct thing * b);
void free_thing(struct thing * thing);
void copy_thing(struct thing * n, struct thing * o);

struct thing * eval_thing(struct thing * thing, struct variable * variables);

void print_thing(struct thing * thing);
char * forward_section(char * str);
struct thing * parse_string(char * str);
