all: xtermview

CC=gcc

LIBS=-lX11 -lXdamage -lXfixes
CFLAGS=-Os -pipe -s -ansi -pedantic -Wall
DEBUGCFLAGS=-Og -pipe -g -ansi -pedantic -Wall

INPUT=xtermview.c
OUTPUT=xtermview

RM=/bin/rm

.PHONY: xtermview
xtermview:
	$(CC) $(INPUT) -o $(OUTPUT) $(LIBS) $(CFLAGS)

debug:
	$(CC) $(INPUT) -o $(OUTPUT) $(LIBS) $(DEBUGCFLAGS)

clean:
	if [ -e $(OUTPUT) ]; then $(RM) $(OUTPUT); fi
