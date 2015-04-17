#mlm


lisp interpretter.
takes lisp expressions from stdin, parses and evaluates.

##types


each atom stores its type. the type is set when the atom is parsed at 
the begining. types are set by the following rules.

####list:
if the first character is '(' it will read to the corrosponding
closing bracket and break it apart.

####char:
first character is a single quote ('), currently there are no
escape codes or unicode support. escape codes will happen. 
unicode might happen.

#####int:
if the string starts with '-' (and has a length greater than 1)
, '0' through to '9' or '.' it is considered to be an integer 
unless it has a '.' then it becomes a double. atoi and atof rules
apply for parsing.

####double:	
read above.

####symbol:	
anything that is not one of the above. it's name is stored for
latter interchanging functions, values or opperators.

#####function: 
special type that is created with lambda (\). 

char's and int's can generally be interchanged except for equality comparisons 
(greater than will work) as the type is compared before anything else. 

##opperators

these are the build in functions.

(+ n n)
	
takes any number of args, takes type of last.

(- n n)
	
take any number of args, take type of last.
(* n n)
	
works the same as -

(/ n n)
	
wors the same as -

(= a b)
	
works with all types.

(> a b)
	
behavior with functions is undefined.

(is a b)
	
returns () if a is not the same type as b. takes any number of args.

(car l)

(cdr l)

(cons thing thing)

(def label thing)
	
adds/sets label to thing in list of available variables.

(\ a b c (+ a b c))

last argument is the function, arguments before that are
names of local variables. the result can be used like an 
operator. define it as something if you want.
(cond (ifnotnil returnthis) (elseifnotnil returnthis))
(include file)
	
parses file so adding any definitions and updating the state.

these are also built in functions but i felt they should be seperate.

(exec path arg1 arg2)

(read match)

(print list)

(error list)

##notes

it is incredibly lazy. it will not evaluate something unless it absolutely 
has too. this includes when printing. so you will probably get back unprocessed
lists.

you can override opperators in list.

if an error occurs functions will generally either return nil or act undefined.
normally nil if it is due to bad list lengths. act straingly if you pass a 
function when it should be an int or something like that.

the state is the return value of the last block.
on exit prints state.

'a' -> ascii value of a

"abc" -> ('a' 'b' 'c') ; TODO.

when evaulating.
if thing is list and list->car is a symbol or evaluates to a symbol/function
we do something. otherwise we just return what was given.

first try substitute symbol for a variable/function. if that fails then 
look through list of opperators. if that fails print error and exit. if 
opperator found then we give rest of list to opperator and return its return 
value. if it was a symbol which was subsituted with a function we give the 
rest of the list to the function.

you will need to have a last new line on your files.