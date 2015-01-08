FILES=mlm.c mlm.h builtinfunctions.c builtinfunctions.h

all: mlmi

mlmi: $(FILES) mlmi.c  iofunctions.c iofunctions.h
	gcc -o mlmi mlmi.c

clean:
	rm mlmi

install: all
	install -Dm 755 mlmi /usr/local/bin/mlmi
	install -Dm 555 lisp/funcs.l /usr/lib/mlm/funcs.l
	install -Dm 555 lisp/iofuncs.l /usr/lib/mlm/iofuncs.l

uninstall: 
	rm /usr/local/bin/mlmi
