ROOT_DIR=$(CURDIR)
CXX+= -std=c++11
CXXFLAGS= -g -O0 -Wall
LEX=flex
YACC=bison

INCLUDES+= -I$(ROOT_DIR)/grammar
LIBS+= 

CXXFLAGS+= $(INCLUDES)

BUILD_DIR=build
PROGRAM=regexp
DEPS=.make.dep

include grammar/Makefile
include src/Makefile

OBJS= $(SRCS:.cpp=.o) $(SRCS:.c=.o) $(SRCS:.cc=.o)

CLEAN_FILE+= $(OBJS) $(BUILD_DIR)/$(PROGRAM) $(DEPS)

clean:
	rm -rf $(CLEAN_FILE)
	

$(DEPS): $(SRCS)
	@$(CXX) $(CXXFLAGS) -MM $(SRCS) > $(DEPS);

all: $(DEPS) $(BUILD_DIR)/$(PROGRAM)

$(BUILD_DIR)/$(PROGRAM): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

include $(DEPS)