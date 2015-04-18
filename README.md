#mlm

a lisp interpretter.
takes lisp expressions from stdin, parses and evaluates.

##types

each atom stores its type. the type is set when the atom is parsed at 
the begining. types are set by the following rules.

####list:
if the first character is '(' it will read to the corrosponding
closing bracket and break it apart.

####char:
first character is a single quote ('), supports most c escape codes.
unicode might happen.

####string:
if first character is a double quote (") then the values between in and
the next double quote and evaulated as char's and put into a list.

####int:
if the string starts with '-' (and has a length greater than 1)
, '0' through to '9' or '.' it is considered to be an integer 
unless it has a '.' then it becomes a double. atoi and atof rules
apply for parsing.

####double:	
read above.

####symbol:	
anything that is not one of the above. it's name is stored for
latter interchanging functions, values or opperators.

####function: 
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

returns the head of the list.

    (cdr l)

returns the tail (everthing that isn't the head) of l.

    (cons x l)

makes a new list with x at the head and l (should be a list or nil) behind it.

    (def label thing)
	
adds/sets label to thing in list of available variables.

    (\ a b c (+ a b c))

last argument is the function, arguments before that are
names of local variables. the result can be used like an 
operator. define it as something if you want.

    (cond (ifnotnil returnthis) (elseifnotnil returnthis))

these are also built in functions but i felt they should be seperate.

*Yet to impliment*

    (exec path arg1 arg2)

    (read match)

    (print list)

    (error list)

##bangs

to include another file do.

	#include stdlib.l

this will include ./stdlib.l if it exists, if it doesn't /usr/share/mlm/stdlib.l
if that fails then an error is printed and mlm exits.

shebangs

	#!/usr/bin/mlm

are ignored so the above is fine to have. so yes. you can do that then exectute the
file like a normal script.

##notes

anything between a semicolor and end of line will be ignored.

it is incredibly lazy. it will not evaluate something unless it absolutely 
has too. this includes when printing. so you will probably get back unprocessed
lists.

you can override opperators with def.

if an error occurs functions will cause the program to die printing a very
non-informative sentance about the problem.

the state is the return value of the last block.
on exit state is printed.

    'a' -> ascii value of a

    "abc" -> ('a' ('b' ('c' ())))
