#
# A simple makefile  
#
# make depend: will build dependency rules for you and add them to end of 
#   		this makefile
# make: build the target executable defined by MAIN 
#
#
.PHONY: depend clean backup setup

# target executable
MAIN=test_prog

# nltk target executable
MAIN_NLTK=test_nltk

# define the compiler
CC= gcc

# define compiler flags
CFLAGS= -Wall -g
#CFLAGS= -p

# paths to include directories for any header files not in (/usr/include)
# here, since I have a header file in this directory,
# I need to specify . as a path to search for .h files:
INCLUDES = -I../includes 

# paths to libraries (for any thing not in /usr/lib)
# ex: -L/home/newhall/mlibs  -L/lib
LFLAGS=

# libraries (-lm for example)
LIBS= -lreadline 

# source files: 
SRCS = main.c ast.c 

# source files: 
NLTK_SRCS = nltk_main.c ast.c 

# target for shared library
SHARED_OBJ= libast.so

SHARED_OBJ_SRCS= ast.c

# the rest of this is generic and will work for any build of single
# executable (just define program-specific things above)

OBJS = $(SRCS:.c=.o)

NLTK_OBJS = $(NLTK_SRCS:.c=.o)

SH_OBJS = $(SHARED_OBJ_SRCS:.c=.o)

all: $(MAIN)  $(MAIN_NLTK)

$(MAIN):  $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(MAIN) $(LFLAGS) $(LIBS)

$(MAIN_NLTK):  $(NLTK_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(NLTK_OBJS) -o $(MAIN_NLTK) $(LFLAGS) $(LIBS)

$(SHARED_OBJ): $(SH_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(SH_OBJS) -shared -o $(SHARED_OBJ) 


.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<


depend: $(SRCS)
	makedepend $(INCLUDES) $^

clean:
	rm -f *.o  $(MAIN)  $(MAIN_NLTK) $(SHARED_OBJ)

backup:
	mkdir bak
	cp Makefile *.[ch] bak

# DO NOT DELETE THIS LINE -- make depend needs it 

