#mlm

It's a lisp interpretter.
Takes input from stdin or files given as arguments.

##types

Each atom stores its type. the type is set when the atom 
is parsed at the begining. types are set by the following 
rules.

####nil:
An empty list, though not stored as one.

####list:
If the first character is '(' it will read to the
corrosponding closing bracket and break it apart. A list 
consists of car and cdr.
The end of the list will always have a nil atom.

####char:
If the first character is a single quote ('). Supports most c
escape codes.
unicode might happen.

####string:
If the first character is a double quote (") then the values
between it and the next double quote and evaulated as char's
and put into a list.

####int:
If the first char is '-' (and has a length greater than 1)
, '0' through to '9' or '.' it is considered to be an integer 
unless it has a '.' (anywhere in it) then it becomes a float.
Atoi and atof rules apply for parsing.

####float:	
Read above.

####symbol:	
Anything that is not one of the above. it's name is stored for
latter interchanging functions, values or opperators.

####function: 
A special type that is created with lambda (\) or in c and added 
as a symbol.

Char's and int's can generally be interchanged except for
equality comparisons (greater than will work) as the type is 
compared before anything else. 

##opperators

these are the build in functions.

the following take any number of args, and returns a thing with the
type of the last. They work with floats, chars and ints. 

    (+ n n)
    (- n n)
    (* n n)
    (/ n n)

The next three are similar execept they have undefined behaviour
with floats as well as lists and functions.

    (| n n)
    (& n n)
    (^ n n)

Just a random gap, don't freak out too much. Okay, it's probably safe
to freak out a little. I'm changing to having the definitions below.
the examples.

    (= a b)
	
Works with all types. Returns 1 if they are equal, () otherwise. 
Works with functions.

    (> a b)
	
Similar to equal. Behavior with functions is undefined.

    (is a b)
	
Returns 1 if a and b have the same type.

Makes a new list with x as the car and l (either a list or nil) as the
cdr.

    (cond (ifnotnil returnthis) (elseifnotnil returnthis))

The rest of the functions have limits on the number of arguments they
take.

    (car l)

Returns the car (first value) of the list.

    (cdr l)

Returns the cdr (everthing that isn't the car) of l.

    (cons x l)

Fairly self explanatory. You can have as many clauses as you wish.

    (def label thing)
	
Adds/sets label to thing in list of available variables.

    (\ a b c (+ a b c))

Creates a new function. a b and c are arguments that can be accessed from
inside the function. You can have as many arguments named as you wish.
The last argument you give lambda is the function.

These are also built in functions but i felt they should be seperate.

    (exec path arg1 arg2)

    (read match)

    (print list)

    (error list)

##bangs

To include another file do.

	#stdlib.l

This will cause the interpreter to look through it's library path for this file.
The library path by default is "." and "/usr/share/mlm" (unless you change that
at compile time). You can add other paths by -Lpath/to/files when you run mlm.
These will be searched after "." but before the default library path.

shebangs

	#!/usr/bin/mlm

Are ignored so the above is fine to have. so yes. you can do that then exectute the
file like a normal script.

##notes

Anything between a semicolor and end of line will be ignored.

It is incredibly lazy. it will not evaluate something unless it absolutely 
has too. this includes when printing. so you will probably get back unprocessed
lists.

You can override all default functions with def.

the state is the return value of the last block.
on exit state is printed.

    "abc" -> ('a' ('b' ('c' ())))
