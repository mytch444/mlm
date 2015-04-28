#define STD_LIBRARY_PATH "/usr/share/mlm"
#define IS_SPACE(X) (X == '\n' || X == '\t' || X == '\r' || X == ' ')


enum type {
	NIL,
	INT,
	FLT,
	CHR,
	LST,
	SYM,
	FNC
};

struct mlm_thing
{
	enum type type;
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

static struct mlm_thing * eval_function(struct mlm_function *f,
                                        struct mlm_thing * args,
				        struct mlm_symbol * symbols);

static struct mlm_thing * eval_thing(struct mlm_thing * thing,
				     struct mlm_symbol * symbols);

static struct mlm_thing * find_symbol(struct mlm_symbol * symbols,
                                      char * name);

static struct mlm_thing * parse_string(char * str);
static char parse_char(char * str);

static void print_thing(struct mlm_thing * thing);

/* Parses data from file descripter fd. Returns the state once finished. */
static struct mlm_thing * parse_file(int fd,
                                     struct mlm_symbol * symbols);

static void add_definition(struct mlm_symbol * s,
                           char * name,
                           struct mlm_thing * t);

static struct mlm_thing * make_operator(
                         int (*func)(struct mlm_thing *,
                                     struct mlm_thing *,
                                     struct mlm_symbol *));

static struct mlm_symbol * make_default_symbols();
