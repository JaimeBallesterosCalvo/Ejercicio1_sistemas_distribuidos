CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -lrt

BIN_SERVER = servidor
LIB_CLIENT = libclaves.so
BIN_CLIENT = cliente

all: $(BIN_SERVER) $(LIB_CLIENT) $(BIN_CLIENT)

$(BIN_SERVER): servidor-mq.o claves.o
		$(CC) $(CFLAGS) servidor-mq.o claves.o -o $(BIN_SERVER) $(LDFLAGS)

$(LIB_CLIENT): proxy-mq.o
		$(CC) -shared $(CFLAGS) -fPIC proxy-mq.o -o $(LIB_CLIENT) $(LDFLAGS)

$(BIN_CLIENT): app-cliente.o $(LIB_CLIENT)
		$(CC) $(CFLAGS) app-cliente.o -L. -lclaves -o $(BIN_CLIENT) $(LDFLAGS) -Wl,-rpath,.

%.o: %.c
		$(CC) $(CFLAGS) -c $<

clean:
		rm -f $(BIN_SERVER) $(LIB_CLIENT) $(BIN_CLIENT) *.o