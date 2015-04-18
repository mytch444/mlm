#define NIL 0
#define INT 1
#define FLT 2
#define CHR 3
#define LST 4
#define SYM 5
#define FNC 6

#define IS_SPACE(X) (X == '\n' || X == '\t' || X == '\r' || X == ' ')

#define N_PATHS 2
char *library_paths[] = {
	".",
	"/usr/share/mlm",
};

struct function
{
	struct thing * thing;
	struct variable * variables;
};

struct thing
{
	int type;
	float value;
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

void die(char * mes);
int thing_equivalent(struct thing * a, struct thing * b);
void free_thing(struct thing * thing);
void copy_thing(struct thing * n, struct thing * o);

struct thing * eval_thing(struct thing * thing, struct variable * variables);

void print_thing(struct thing * thing);
char * char_list_to_string(struct thing * thing);
int check_section_end(char c);
char parse_char(char * str);
struct thing * parse_string(char * str);
void parse_hash(char * str, struct variable * variables);
struct thing * parse_file(int fd, struct thing * state, struct variable * variables);
