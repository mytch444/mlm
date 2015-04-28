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
to freak out a little. I'm changing to having the definitions below
the examples.

    (= a b)
	
Works with all types. Returns 1 if they are equal, () otherwise. 
Works with functions.

    (> a b)
	
Similar to equal. Behavior with functions is undefined.

    (is a b)
	
Returns 1 if a and b have the same type.

    (cond
    	(ifnotnil returnthis)
    	(elseifnotnil returnthis))

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
    
    (\ (a b c) (+ a b c))

Creates a new function. a b and c are arguments that can be accessed 
from inside the function. It returns the value of the last statement.
So yes, you can have more than one statement like the following

    (\ (a b)
    	(exec "/bin/echo" (to-string a))
    	(exec "/bin/echo" (to-string b))
    	(+ a b)))

Which (if you created to-string) will echo a, then echo b, then return a + b.

    (exec path arg1 arg2)

Forks and executes path with arguments arg1 arg2...

##Notes

Anything between a hash and end of line will be ignored.

You can override all default functions with def.

The state is the return value of the last block.
on exit state is printed.

    "abc" -> ('a' ('b' ('c' ())))
