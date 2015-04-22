#define STD_LIBRARY_PATH "/usr/share/mlm"

#define NIL 0
#define INT 1
#define FLT 2
#define CHR 3
#define LST 4
#define SYM 5
#define FNC 6

#define IS_SPACE(X) (X == '\n' || X == '\t' || X == '\r' || X == ' ')

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
	struct mlm_thing * thing;
	int (*func)(struct mlm_thing * result,
	             struct mlm_thing * args,
	             struct mlm_symbol * symbols);};

struct mlm_symbol
{
	char * name;
	struct mlm_thing * thing;
	struct mlm_symbol * next;
};

/* Returns 0 if not equivalent */
static int thing_equivalent(struct mlm_thing * a, struct mlm_thing * b);

/* Free thing, if thing is a list free's all things in list. */
static void free_thing(struct mlm_thing * thing);

/* Copy properties (including car and cdr) for o to n. */
static void copy_thing(struct mlm_thing * n, struct mlm_thing * o);

/* Evaluate thing with symbols avaliable */
static struct mlm_thing * eval_thing(struct mlm_thing * thing, struct mlm_symbol * symbols);

static struct mlm_thing * find_symbol(struct mlm_symbol * symbols, char * name);

static struct mlm_thing * parse_string(char * str);
static char parse_char(char * str);

static void print_thing(struct mlm_thing * thing);

/* Parses data from file descripter fd. Returns the state once finished. */
static struct mlm_thing * parse_file(int fd, int inter,
                                     struct mlm_path * paths, struct mlm_symbol * symbols);

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

static void add_operator_next(struct mlm_symbol * s,
                         char * name,
                         int (*func)(struct mlm_thing *,
                                     struct mlm_thing *,
                                     struct mlm_symbol *));

static struct mlm_symbol * make_default_symbols();
