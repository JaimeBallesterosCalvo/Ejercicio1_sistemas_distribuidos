CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -lrt

BIN_SERVER = servidor
LIB_CLIENT = libclaves.so
BIN_CLIENT = cliente

# Añade esta línea para los flags específicos de objetos que irán a bibliotecas compartidas
PIC_FLAGS = -fPIC

all: $(BIN_SERVER) $(LIB_CLIENT) $(BIN_CLIENT)

$(BIN_SERVER): servidor-mq.o claves.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(LIB_CLIENT): proxy-mq.o
	$(CC) -shared $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_CLIENT): app-cliente.o $(LIB_CLIENT)
	$(CC) $(CFLAGS) $< -L. -lclaves -o $@ $(LDFLAGS) -Wl,-rpath,.

# Regla especial para proxy-mq.o con -fPIC
proxy-mq.o: proxy-mq.c
	$(CC) $(CFLAGS) $(PIC_FLAGS) -c $< -o $@

# Regla genérica para otros .o
%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(BIN_SERVER) $(LIB_CLIENT) $(BIN_CLIENT) *.o