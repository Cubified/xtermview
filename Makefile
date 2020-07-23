all: xtermview

CC=gcc

LIBS=-lX11 -lXdamage -lXfixes
CFLAGS=-Os -pipe -s
DEBUGCFLAGS=-Og -pipe -g

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
