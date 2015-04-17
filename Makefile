mlm: mlm.c mlm.h operators.c
	gcc -o mlm mlm.c

.PHONY: clean
clean: 
	rm mlm