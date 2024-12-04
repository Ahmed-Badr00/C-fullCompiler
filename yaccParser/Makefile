CC=gcc
CFLAGS=-Wall -g

parser: y.tab.c lex.yy.c
	$(CC) $(CFLAGS) -o parser y.tab.c lex.yy.c -lfl

y.tab.c: parser.y
	yacc -d -v parser.y

lex.yy.c: scanner.l
	flex scanner.l

clean:
	rm -f parser lex.yy.c y.tab.c y.tab.h y.output *.o