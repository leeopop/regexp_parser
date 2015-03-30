ROOT_DIR=$(CURDIR)

include grammar/Makefile

clean:
	rm -rf $(CLEAN_FILE)

all: $(GRAMMAR_SRC)