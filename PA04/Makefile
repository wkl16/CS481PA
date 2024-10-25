CC=gcc
CFLAGS= -Wall -pedantic -O0
SRCS=$(shell find . -name '*.c')
BINS=$(patsubst %.c,%,$(SRCS))
DIR=$(shell basename $(CURDIR))
GITIGNORE=.gitignore
RM=rm -f

all: $(BINS)

$(GITIGNORE): FORCE
	$(shell echo $(patsubst ./%,$(DIR)/%,$(BINS)) | sed -e 's/ /\n/g' >> $@)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(BINS)

FORCE:
