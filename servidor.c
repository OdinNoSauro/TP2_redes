#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <string.h>
#include <sys/time.h>
#include <netinet/in.h>

#include "tp_socket.h"

#define AMOSTRAS 100


int main (int argc, char *argv[]){
	tp_init();

	int PORTA_SERVIDOR = atoi(argv[1]); // porta da conexão
	int LENGTH = atoi(argv[2]); // tamanho do buffer
	char nomeArq[20];
	int i = 0;
	int bytes_lidos,bytes_sendto;
	char *buffer = malloc(LENGTH*sizeof(char));
	int socket_des; // descritor do socket
	int bytes_enviados = 0;

	struct timeval inicio, fim;
	float media = 0;

	so_addr cliente;

	socket_des = tp_socket(PORTA_SERVIDOR);
	if (socket_des == -1){
		perror("socket ");
		exit(1);
	}else if(socket_des == -2){
		perror("build addr");
		exit(1);
	}else if (socket_des == -3){
		perror("bind");
		exit(1);
	}else
	printf("Socket criado com sucesso\n");

	memset(buffer, 0x0, LENGTH);
	memset(nomeArq, 0x0, 20);

	//Recebe nome do arquivo
	unsigned int sock_len = sizeof(struct sockaddr_in);
	do {
		tp_recvfrom(socket_des,buffer,sizeof(char), &cliente);
		nomeArq[i] = buffer[0];
		i++;
	} while(buffer[0]!='\0');
	printf("Nome do arquivo: %s\n", nomeArq);

	// Envia arquivo
	FILE* fp = fopen((const char*) nomeArq, "r");
	memset(buffer, 0x0, LENGTH);
	while((bytes_lidos=fread(buffer,sizeof(char),LENGTH, fp)) > 0){
		bytes_sendto = tp_sendto(socket_des, buffer, bytes_lidos, &cliente);
		bytes_enviados+=bytes_sendto;
		memset(buffer, 0x0, LENGTH);
	};

	//Encerra e limpa a memória
	printf("Conexão encerrada\n");
	fclose(fp);
	close(socket_des);
	free(buffer);
	return 0;
}
