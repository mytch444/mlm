EXEC=mlm

all: mlm

mlm: mlm.c builtinfunctions.c
	gcc -o $(EXEC) mlm.c

clean:
	rm $(EXEC)
