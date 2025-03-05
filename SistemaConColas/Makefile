CC = gcc
CFLAGS =  -g -Wall
BIN_FILES  = cliente-servidor

all: $(BIN_FILES)

cliente-servidor: app-cliente.o claves.o
	$(CC) $(CFLAGS) $^  -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(BIN_FILES) *.o

