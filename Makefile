BINARY=./bin/main
INCLUDEDIRS=. ./include
SRCDIRS=. ./src ./lib

CC=gcc
OPT=-O0
# Tell make to track header file changes
DEPFLAGS=-MP -MD
WARNINGS=-Wall -Wextra -pedantic
# Tell make to load specified libraries and add /usr/local/include to ld path
LDLIBS=-lSDL2
LDFLAGS=-L/usr/local/include
CFLAGS=$(WARNINGS) -g -std=c2x $(foreach D,$(INCLUDEDIRS),-I$(D)) $(OPT) $(DEPFLAGS)

# Loop through all source dirs, glob for *.c files
SRCFILES=$(foreach X,$(SRCDIRS),$(wildcard $(X)/*.c))
OBJECTS=$(patsubst %.c,%.o,$(SRCFILES))
DEPFILES=$(patsubst %.c,%.d,$(SRCFILES))

all: $(BINARY)

$(BINARY): $(OBJECTS)
	[ -d "bin" ] || mkdir bin
	$(CC) $(LDFLAGS) $(LDLIBS) -o $@ $^

# Only want the first dependency here, so use $< instead of $^
# Note we're using wildcards instead of the variables, in order to match exactly
# one object file with its sole *.c counterpart
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BINARY) $(OBJECTS) $(DEPFILES)
	[ -f "$(BINARY)" ] || rmdir bin

distribute: clean
	tar -czvf *
