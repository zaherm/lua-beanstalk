UNAME := $(shell uname)
all-tests := $(basename $(wildcard test/test_*.lua))

LIB_PATH= /usr/lib64
INC_PATH= /usr/include
BIN_PATH= /usr/bin
LUA_LIB= -L$(LIB_PATH) -llua
LUA_INC= -I$(INC_PATH) -I$(INC_PATH)/lua
BEANSTALK_LIB= -lbeanstalk
BEANSTALK_INC= -I$(INC_PATH)/beanstalk
EXTRACFLAGS= -std=c99 -undefined -fPIC

# change these based on your installation
ifeq ($(UNAME), Darwin)
	LIB_PATH= /usr/local/lib
	INC_PATH= /usr/local/include
	BIN_PATH= /usr/local/bin
	LUA_LIB= -L$(LIB_PATH) -llua5.1
	LUA_INC= -I$(INC_PATH) -I$(INC_PATH)/lua-5.1
	BEANSTALK_LIB= -lbeanstalk
	BEANSTALK_INC= -I$(INC_PATH)/beanstalk
	EXTRACFLAGS= -std=c99 -undefined dynamic_lookup -fPIC
endif


INC= $(LUA_INC) $(BEANSTALK_INC)
LIB= $(LUA_LIB) $(BEANSTALK_LIB)
WARN= -Wall
CFLAGS= -O2 $(WARN) $(INC)

LIBNAME= beanstalk
T= $(LIBNAME).so
OBJS= src/l$(LIBNAME)_aux.o \
			src/l$(LIBNAME).o

all: $T

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

$T:	$(OBJS)
	$(CC) $(CFLAGS) $(LIB) $(EXTRACFLAGS) -o $@ -shared $(OBJS)

clean:
	rm -f $T $(OBJS)

clean-all:
	rm -f $T $(OBJS)

tests: $(all-tests)

test/test_%: $T
	lua $@.lua

test: clean all tests

install: $(TARGET)


