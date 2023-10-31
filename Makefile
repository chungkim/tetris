TARGET = tetris
#LIBS = -lncurses -ltinfo
LIBS = -lncurses
CC = clang
CFLAGS = -g -Wall -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=600

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -rf *.dSYM
	-rm -f $(TARGET)

deps:
	sudo apt-get update
	sudo apt-get install libncurses5-dev libncursesw5-dev
