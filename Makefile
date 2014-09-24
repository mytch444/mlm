FILES=atom.c atom.h builtinfunctions.c builtinfunctions.h iofunctions.c iofunctions.h evaluate.c evaluate.h parse.c parse.h repl.c repl.h

all: mlm

mlm: $(FILES) mlm.c
	gcc -o mlm mlm.c

clean:
	rm mlm
