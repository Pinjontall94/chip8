BINARY=./bin/main
INCLUDEDIRS=. ./include
SRCDIRS=. ./src ./lib

CC=gcc
OPT=-O0
# Tell make to track header file changes
DEPFLAGS=-MP -MD
WARNINGS=-Wall -Wextra -Werror -pedantic -std=c99
# Tell make to load specified libraries and add /usr/local/include to ld path
LDLIBS=-lSDL2 -lm
LDFLAGS=-L/usr/local/include
CFLAGS=$(WARNINGS) -g -std=c99 $(foreach D,$(INCLUDEDIRS),-I$(D)) $(OPT) $(DEPFLAGS)

# Loop through all source dirs, glob for *.c files
SRCFILES=$(foreach X,$(SRCDIRS),$(wildcard $(X)/*.c))
OBJECTS=$(patsubst %.c,%.o,$(SRCFILES))
DEPFILES=$(patsubst %.c,%.d,$(SRCFILES))

.PHONY: all
all: $(BINARY)

.PHONY: run
run:
	$(BINARY)

$(BINARY): $(OBJECTS)
	[ -d "bin" ] || mkdir bin
	$(CC) $(LDFLAGS) $(LDLIBS) -o $@ $^

# Only want the first dependency here, so use $< instead of $^
# Note we're using wildcards instead of the variables, in order to match exactly
# one object file with its sole *.c counterpart
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -rf $(BINARY) $(OBJECTS) $(DEPFILES)
	[ -f "$(BINARY)" ] || rmdir bin

distribute: clean
	tar -czvf *
