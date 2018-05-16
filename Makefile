############################# Makefile ##########################

CLIENTE_SRC = cliente.o tp_socket.o
SERVIDOR_SRC = servidor.o tp_socket.o

all: cliente servidor

cliente: $(CLIENTE_CSRC)
	gcc -o cliente cliente.c tp_socket.c -lm

cliente.o: cliente.c
	gcc -c cliente.c -w -Wall -std=c++98

tp_socket.o: tp_socket.c
	gcc -c tp_socket.c -w -Wall -std=c++98

servidor: $(SERVIDOR_SRC)
	gcc -o servidor $(SERVIDOR_SRC) -lm

servidor.o: servidor.c
	gcc -c servidor.c -Wall

clean:
	rm -rf *.o

mrproper: clean
	rm -rf cliente servidor
