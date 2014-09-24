#define isspace(X) (X == ' ' || X == '\n' || X == '\t')
#define isspecial(X) (X == '(' || X == ')' || X == '\"')
#define ischar(X) (!isspace(X) && !isspecial(X))
#define iscomment(X) (X == ';')
#define LINELENGTH 10000

int closing_bracket_pos(char *string, int open);
char *string_cut(char *string, int start, int stop);

int string_is_int(char *string);
int string_is_float(char *string);
int string_is_char(char *string);
int string_is_string(char *string);

symbol *find_symbol(char *name);
atom *swap_symbols(atom *atoms);

atom *data_to_atom(data *d);

data *char_to_data(char c);
data *int_to_data(int i);
data *float_to_data(double f);

data *string_to_data(char *string);

atom *string_to_atom_string(char *string);
atom *constant_to_atom(char *name);

atom *parse(char *string);
atom *read_expression(FILE *f);
