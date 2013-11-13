
# Makefile - very simple template for single executable programs
#	     from C source and header files.
#

# Define standard variables for the compiler.
# We use CPP_FLAGS, C_FLAGS, LD_FLAGS as our own variables and let the
# conventional CPPFLAGS, CFLAGS and LDFLAGS for the user to pass them
# at the command line.  Since we then use *_FLAG before *FLAG, the user
# can eventually overwirte our flags with his own flags.

CC = gcc #			#The complier (example works for gcc)
CPP_FLAGS = # 			#Preprocessor flags
C_FLAGS   = -Wall -std=c99 -pedantic -D_POSIX_SOURCE#		#Compiler flags
LD_FLAGS  = #			#Linker flags

#
# The variables you shall change
#

binary = ShellBacca			   #The executable program to be build
objects = main.o parse.o shell.o historico.o LinuxUtils.o signal_handler.o linha_comando.o stringcmd.o  #List of objects that will be linked

garbage = \#* *~		   # Garbage to clean up, e.g. temp files

#
# You won't probably need to change bellow this line
#


all : $(binary)

# Pattern rule to build objects from sources

%.o : %.c
	$(CC) $(CPP_FLAGS) $(C_FLAGS) $(CPPFLAGS) $(CFLAGS) -c $<  

# For each file.o in $(object) we want to add a line in the form
# 'file.o: header1.h header2.h ... headerN.h'
# where header*.h are the headers included by file.c
# Gcc's -MM flags does the trick for us: we use it to create an auxiliary
# makefile file.d for each file.o and than include all auxiliary makefiles
# in this very Makefile.  Actually, almost ready: if the source.c or any
# header*.h is change as to include another headerN+1.h, then we need to
# regenerate file.d.  Gcc's flat -MM in conjunction with -MT does the trick 
# and we get a line in the form 'file.o file.d : file.c header1.h ... headerM`
# Exactly what we need.

makefiles = $(objects:.o=.d) 	# auxiliary makefiles, one for each object

# Pattern rule for makefiles using gcc -MM -MT flag
# anything.c will depend on anything.d, $< stands for the first pre-requisite
# and $@ stands for the target. $(something:token=other) is $(something) with
# the substring 'token' replaced with 'other'.

%.d: %.c
	$(CC) -MM -MT '$(<:.c=.o) $@' $(CPPFLAGS) $< > $@

# Including the object's dependencies on headers

include $(makefiles)

# The rule to build the binary

$(binary) : $(objects)
	$(CC) $(LD_FLAGS) $(LDFLAGS) $(objects) -o $@

# Clean up the mess: objects, makefiles, binary and garbage
# It's a PHONY tag so make does not even check for a file named 'clean'
# It not only avoid unecessary file system access but it's also robust
# in the pathological case there is a file named 'clean' in the directory.

.PHONY clean:
	rm -f *.o *.d $(binary) $(garbage)

