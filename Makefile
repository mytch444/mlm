FILES=mlm.c mlm.h builtinfunctions.c builtinfunctions.h

all: mlmi

mlmi: $(FILES) mlmi.c  iofunctions.c iofunctions.h
	gcc -o mlmi mlmi.c

clean:
	rm mlmi

