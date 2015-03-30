ROOT_DIR=$(CURDIR)
CXX+= -std=c++11
CXXFLAGS= -g -O0 -Wall
CFLAGS= -g -O0
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

OBJS= $(SRCS:.cpp=.o)
C_OBJS= $(C_SRCS:.c=.o)

CLEAN_FILE+= $(OBJS) $(C_OBJS) $(BUILD_DIR)/$(PROGRAM) $(DEPS)

clean:
	rm -rf $(CLEAN_FILE)
	

$(DEPS): $(SRCS) $(C_SRCS)
	@$(CXX) $(CXXFLAGS) -MM $(SRCS) > $(DEPS);
	@$(CC) $(CFLAGS) -MM $(C_SRCS) >> $(DEPS);

all: $(DEPS) $(BUILD_DIR)/$(PROGRAM)

$(BUILD_DIR)/$(PROGRAM): $(OBJS) $(C_OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(C_OBJS) -o $@

include $(DEPS)