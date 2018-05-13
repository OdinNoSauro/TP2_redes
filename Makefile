

output: tp_socket.o servidor.o
	gcc tp_socket.o servidor.o -o output

tp_socket.o: tp_socket.c tp_socket.h 
	gcc	-c	tp_socket.c
servidor.o:	servidor.c 
	gcc	-c servidor.c

clean:	
	@rm -f *.o output
mrproper:	clean
	rm -f output
