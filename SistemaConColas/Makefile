CC = gcc
CFLAGS = -g -Wall -pthread -fPIC # Añadimos -fPIC para la librería dinámica
LDFLAGS = -lrt # Añadimos -lrt para las colas de mensajes
CC = gcc
CFLAGS = -g -Wall -pthread -fPIC
LDFLAGS = -lrt
BIN_SERVER = servidor
LIB_CLIENT = libclaves.so
BIN_CLIENT = app-cliente

all: $(BIN_SERVER) $(LIB_CLIENT) $(BIN_CLIENT)

$(BIN_SERVER): servidor-mq.o claves.o
		$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(LIB_CLIENT): proxy-mq.o
		$(CC) -shared $(CFLAGS) $< -o $@ $(LDFLAGS)

$(BIN_CLIENT): app-cliente.o $(LIB_CLIENT)
		$(CC) $(CFLAGS) app-cliente.o -L. -lclaves -o $@ $(LDFLAGS)

%.o: %.c
		$(CC) $(CFLAGS) -c $<

clean:
		rm -f $(BIN_SERVER) $(LIB_CLIENT) $(BIN_CLIENT) *.o

