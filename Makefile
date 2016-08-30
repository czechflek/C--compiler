#
# Top level makefile for building lexer, parser, and codegen executables
#
#  make: build executables
#  make clean: make clean everywhere 
#  make depend: make depend everywhere 
#

allmodules = lexer parser ast codegen   

# make command line options:
# -f filename:  use filename as the makefile
# -C directory: change to directory before reading the makefile
# +@for ...
#    +: execute this line (even if -n option used)
#    @: suppress output of this line 
# (see the man page for make for more information)
#
all: 
	+@for submodule  in $(allmodules); do      \
            if [ -f $$submodule/Makefile ]; then    \
                $(MAKE) -C $$submodule ;            \
            else                                    \
                true;                               \
            fi                                      \
        done



clean depend: 
	+@for submodule in $(allmodules); do       \
            if [ -f $$submodule/Makefile ]; then    \
                $(MAKE) -C $$submodule $@;          \
            else                                    \
                true;                               \
            fi                                      \
        done

