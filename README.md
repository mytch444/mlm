mlm
===

My lisp interpreter.

A simple (ok, it's probably an overly complicated, not functional piece of shit) lisp interpreter.
It's currently being developed. It is being made in such a way that is should be easy to use for adding lisp scripting to something. Like what emacs does. Not sure how to explain that.
It will uses my own dialect of lisp that takes some influences from haskel.

No idea what I'll use it for.

Here's a bad description of how to do shit.

Language
========

Everything is an atom.
An atom is either a function, data, a list or a symbol. I cannot be more than one of these. It is structured like this. 

atom - next -> another atom (next in the list)
     - data
     - list
     - function
     - symbol

Nil is a special type of atom. It is any atom that doesn't have data, a list, a function or a symbol (can have next).

Data has a type (int, char or float (actually a double) at the moment) and it's value, either an int or double.

Lists are just atoms that are joined together by the atoms next thingime.

Functions can be defined with lambda. They have a list of arguments (which should all be symbols to nothing), and an expression. When a function is at the start of a list the rest of the list up to the number of arguments it takes is given to the function which are then swaped in and it is evaluated. If you give a function more arguments that it is ment to have the rest are just ignored.

Symbols can be defined with define and are then constant for the rest of the duration of the interpreters run time. They can be redefined.

Functions
=========
Currently the bult in functions are

Name, number of arguments, explanation.
      -- These should be self explanatory.
      "+", 2, adds
      "-", 2, subtracts from the first argument
      "*", 2, multiplies
      "/", 2, divides
      "=", 2, check for equality for all but end nil.
      ">", 2, check if first arg is greater than second.
      "<", 2, check if first arg is less than second.

      "to-int", 1, casts it's argument to an int.
      "to-float", 1, casts it's argument to a float.
      "to-char", 1, casts it's argument to a char.
      
      "int?", 1, checks if atom is int.
      "float?", 1, checks if atom is float.
      "list?", 1, checks if atom is list.
      "nil?", 1, checks if atom is nil.

      "list", 0+, creates a list from arguments.
      "cons", 2, makes second argument the next of first.
      "car", 1, returns first atom in list.
      "cdr", 1, returns next from list.

      "exit", 0, exits the interperater.

      "eval", 1, evaluates the expression in the string given to it.
      "include", 1, reads and evaluates the file given to it as a string.

      -- These will be explained. 
      "cond", 1+, explained later.
      "define", 2, defines atom to symbol.
      "\", 2, lambda, creates a function.
      "progn", 0+, now lets just ignore the whole functional thing for a moment.

      -- IO functions.
      "print", 1+, prints stuff.
      "read", 0, reads in characters until the user hits enter then returns a list of those characters.

Those ones are built into the language.      
There are a whole heap of more usefull ones in funcs.l.

Cond
====
Use:
	(cond ((= b 0) 1)
	      ((else) 2))

Cond goes through each of its arguments. each argument should be a list of two arguments. The first can be anything, if it is not nil then cond evaluates and returns the second. So in the above example if b was 0 it would return 1, else (a function that returns 1 defined in test.l) it returns 2.

Makes sense? Probably not, that isn't your fault. I'm bad at explaining things.

Define
======
Use:
	(define hi "Hello world")

Define associates a name with the next atom in its args (a list of chars in this case). Can be used with lambda to give names to functions and then use them latter.

Lambda
======
Use:
	(\ (a) (* a a))

Lambda takes two arguments, the first is a list of symbols and the second which is an expression that uses the arguments to return a value. So the above returns a function that when given a single argument returns that argument squared.

Print
=====
Use:
	(print "What a good %'th of % it is!" 10 "March")

Will print out "What a good 10'th of march it is!" to stdout and return 1.
So it swapps all the %'s with the argument that many %'s in. %% will print out %.

Progn
=====
Use:
	(progn
		(print "Hello")
		(print "How are you today")
		(print "Who cares about functional things"))

Ignores everything about functional and just evaluates each of the expressions given to it in set then returns 1.

Syntax
======

An example:

(define else 1)

(define map
  (\ (f l)
   (cond ((nil? l) ())
	 (else (cons
		(f (car l))
		(map f (cdr l)))))))

(map (\ (a) (+ a 1)) (1 2 3))

"hello" is syntactic sugar for ('h' 'e' 'l' 'l' 'o' ()).

Other than that it's pretty much lisp. Or scheme, probably more similar to scheme.

The files funcs.l and test.l are probably better examples of code. They work.
You can load them into the interpreter by giving them as arguments when you run it.

Interpretter
============
The interpreter is the executable mlmi binary that magically shows up when you run make. Or is probably already there due to my lazyness, you'll probably want to run make before trying to use it depending on your archecture.
It is a c program that takes input, evaluates and prints the result.

You can give it files to evaluate by just giving them to it as arguments.

It can read multi line expressions and will only execute on enter if there are an equal number of open and close brackets and not as part of a quote.

There may at some point be a compiler. I recon that would be pretty cool.

