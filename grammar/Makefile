LEX=flex
YACC=bison

all: regexp_yacc.c regexp_yacc.h regexp_lex.c regexp_lex.h

clean:
	rm -rf *.c *.h *.o

regexp_yacc.c regexp_yacc.h: regexp.y
	$(YACC) --defines=regexp_yacc.h --output=regexp_yacc.c regexp.y
	
regexp_lex.c regexp_lex.h: regexp.l
	$(LEX) --header-file=regexp_lex.h --outfile=regexp_lex.c -Cr regexp.l