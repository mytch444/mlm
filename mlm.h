#define NIL 0
#define INT 1
#define FLT 2
#define CHR 3
#define LST 4
#define SYM 5
#define FNC 6

#define IS_SPACE(X) (X == '\n' || X == '\t' || X == '\r' || X == ' ')

#ifndef STD_LIBRARY_PATH
#define STD_LIBRARY_PATH "/usr/share/mlm"
#endif

struct mlm_path
{
	char * path;
	struct mlm_path * next;
};

struct mlm_thing
{
	int type;
	int value;
	float point;
	char * label;
	struct mlm_thing * car;
	struct mlm_thing * cdr;
	struct mlm_function * function;
};

struct mlm_function
{
	struct mlm_symbol * symbols;
	/* A function will either have thing which is a copy of the list
	 * given to lambda as the function */
	struct mlm_thing * thing;
	/* Or a pointer to a function of this type */
	void (*func)(struct mlm_thing * result,
	             struct mlm_thing * args,
	             struct mlm_symbol * symbols);
};

struct mlm_symbol
{
	char * name;
	struct mlm_thing * thing;
	struct mlm_symbol * next;
};

/* Look in operators.c and mlm.c if you want a reason for this */
struct mlm_symbol mlm_null_symbol = { NULL, NULL, NULL };
#define MLM_NEW_FUNCTION_THING(f, func, thing) \
    struct mlm_function func = { &mlm_null_symbol, NULL, &f }; \
    struct mlm_thing thing = { FNC, 0, 0, NULL, NULL, NULL, &func };

static int thing_equivalent(struct mlm_thing * a, struct mlm_thing * b);

static void free_thing(struct mlm_thing * thing);

static void copy_thing(struct mlm_thing * n, struct mlm_thing * o);

static struct mlm_thing * eval_thing(struct mlm_thing * thing, struct mlm_symbol * symbols);

static struct mlm_thing * find_symbol(struct mlm_symbol * symbols, char * name);

static char * char_list_to_string(struct mlm_thing * thing);

static struct mlm_thing * parse_string(char * str);

/* Parses data from file descripter fd. Returns the state once finished. */
static struct mlm_thing * parse_file(int fd, struct mlm_path * paths, struct mlm_symbol * symbols);

/* Parses file from name, it will check for name in the list of library paths,
 * starting with ./ then /usr/share/mlm then going through -Lpath options. 
 * Any symbols defined will be appended to symbols. 
 * This can also be called by adding 
 * #name
 * somewhere (the begining is a good place) in lisp files. The only rule then
 * is that the file name does not start with a !. 
 * If the first char of the name is / then the absolute path is used.
 */
static int include_file(char * name, struct mlm_path * paths, struct mlm_symbol * symbols);

static void add_default_symbols(struct mlm_symbol * symbols);

void print_thing(struct mlm_thing * thing);
int check_section_end(char c);
char parse_char(char * str);
void die(char * mes);
