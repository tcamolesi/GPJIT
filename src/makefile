CC=gcc
CFLAGS=-c -Wall -g
LDFLAGS=-g $(shell allegro-config --libs)
SOURCES=assembler.c element.c ettree.c ga.c main.c
HEADERS=assembler.h element.h ettree.h ga.h geneticalgorithm.h
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=gpjit

all: $(EXECUTABLE)

tags: $(SOURCES) $(SOURCESCLI)
		ctags -R

clean:
		rm -f $(OBJECTS) $(EXECUTABLE) tags

$(EXECUTABLE): $(OBJECTS)
		$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o: $(HEADERS)
		$(CC) $(CFLAGS) $< -o $@
