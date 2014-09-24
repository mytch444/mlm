atom *evaluate(atom *raw);

atom *copy_atom(atom *a);
function *copy_function(function *f);
data *copy_data(data *a);
int isclean(atom *a);
atom *flatten(atom *o);
atom *do_sub(atom *a);
char *atom_to_string(atom *a);
