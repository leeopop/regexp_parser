
GRAMMAR_DIR=$(ROOT_DIR)/grammar

GRAMMAR_SRC+=$(GRAMMAR_DIR)/regexp_yacc.c $(GRAMMAR_DIR)/regexp_yacc.h $(GRAMMAR_DIR)/regexp_lex.c $(GRAMMAR_DIR)/regexp_lex.h
CLEAN_FILE+= $(GRAMMAR_DIR)/*.c $(GRAMMAR_DIR)/*.h $(GRAMMAR_DIR)/*.o


$(GRAMMAR_DIR)/regexp_yacc.c $(GRAMMAR_DIR)/regexp_yacc.h: $(GRAMMAR_DIR)/regexp.y
	$(YACC) --defines=$(GRAMMAR_DIR)/regexp_yacc.h --output=$(GRAMMAR_DIR)/regexp_yacc.c $(GRAMMAR_DIR)/regexp.y
	
$(GRAMMAR_DIR)/regexp_lex.c $(GRAMMAR_DIR)/regexp_lex.h: $(GRAMMAR_DIR)/regexp.l
	$(LEX) --header-file=$(GRAMMAR_DIR)/regexp_lex.h --outfile=$(GRAMMAR_DIR)/regexp_lex.c -Cefr $(GRAMMAR_DIR)/regexp.l
	

C_SRCS+= $(GRAMMAR_SRC)