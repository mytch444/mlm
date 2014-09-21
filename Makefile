FILES=mlm.c atom.c atom.h builtinfunctions.c builtinfunctions.h evaluate.c evaluate.h parse.c parse.h

all: mlm

mlm: $(FILES) mlmc.c mlmi.c
	gcc -o mlmc mlmc.c
	gcc -o mlmi mlmi.c

clean:
	rm mlmc.c
	rm mlmi.c
