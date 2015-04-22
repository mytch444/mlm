#mlm

It's a lisp interpretter.
Takes input from stdin or files given as arguments.

##Types

Each atom stores its type. the type is set when the atom 
is parsed at the begining. types are set by the following 
rules.

####Nil:
An empty list, though not stored as one.

####List:
If the first character is '(' it will read to the
corrosponding closing bracket and break it apart. A list 
consists of car and cdr.
The end of the list will always have a nil atom.

####Char:
If the first character is a single quote ('). Supports most c
escape codes.
unicode might happen.

####String:
If the first character is a double quote (") then the values
between it and the next double quote and evaulated as char's
and put into a list.

####Int:
If the first char is '-' (and has a length greater than 1)
, '0' through to '9' or '.' it is considered to be an integer 
unless it has a '.' (anywhere in it) then it becomes a float.
Atoi and atof rules apply for parsing.

####Float:	
Read above.

####Symbol:	
Anything that is not one of the above. it's name is stored for
latter interchanging functions, values or opperators.

####Function: 
A special type that is created with lambda (\) or in c and 
added as a symbol.

Char's and int's can generally be interchanged except for
equality comparisons (greater than will work) as the type is 
compared before anything else. 

##Functions

These are the build in functions.

The following take any number of args, and returns a thing with 
the type of the last. They work with floats, chars and ints. 

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

    (cond (ifnotnil returnthis) (elseifnotnil returnthis))

Fairly self explanatory. You can have as many clauses as you wish.

The rest of the functions have limits on the number of arguments they
take.

    (car l)

Returns the car (first value) of the list.

    (cdr l)

Returns the cdr (everthing that isn't the car) of l.

    (cons x l)

Makes a new list with x as the car and l (either a list or nil) as the
cdr.

    (def label thing)
	
Adds/sets label to thing in list of available variables.
    
    (\ a b c (+ a b c))

Creates a new function. a b and c are arguments that can be 
accessed from inside the function. You can have as many
arguments named as you wish. The last argument you give 
lambda is the function.

These are also built in functions but i felt they should be 
seperate.

    (exec path arg1 arg2)

Forks and executes path with arguments arg1 arg2...

    (read)

    (write char)

Read/write to files descriptors defined by symbols names 
stdin/stdout respectively. Because of my stupidity you will
have to define stdin/out before you can use these.

    (open path options)

Returns a file descripter for file at path opened with 
options (man 2 open).

##Includes

To include another file do.

	#stdlib.l

This will cause the interpreter to look through it's library
path for this file. The library path by default is "." and 
"/usr/share/mlm" (unless you change that at compile time). You 
can add other paths by -Lpath/to/files when you run mlm. These 
will be searched after "." but before the default library path.

######Shebangs

	#!/usr/bin/mlm

Are ignored so the above is fine to have. so yes. you can do 
that then exectute the file like a normal script.

##Notes

Anything between a semicolor and end of line will be ignored.

It is incredibly lazy. it will not evaluate something unless it
absolutely has too. this includes when printing. so you will 
probably get back unprocessed lists.

You can override all default functions with def.

the state is the return value of the last block.
on exit state is printed.

    "abc" -> ('a' ('b' ('c' ())))

#mlm

##Options

	-Lpath

Adds path to library paths.

	-lname.l

Includes file name.l from library paths.
