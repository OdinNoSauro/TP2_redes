#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>

#include "tp_socket.h"

#define AMOSTRAS 100

struct sockaddr_in servidor;
struct sockaddr_in cliente;
int tamanho = sizeof(cliente);


int main (int argc, char *argv[]){

	int PORTA = atoi(argv[1]); // porta da conexão
	int LENGTH = atoi(argv[2]); // tamanho do buffer
	char nomeArq[20];
	char buffer [LENGTH];
	struct timeval inicio, fim;
	float media = 0;
	int bytes_lidos,bytes_sendto;
	char *aux = malloc(LENGTH*sizeof(char));
	int socket_des; // descritor do socket
	int bytes_enviados = 0;
	tp_init;
	socket_des = tp_socket(htons(PORTA));
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


	printf("Aguardando resposta \n");

	memset(buffer, 0x0, LENGTH);
	memset(nomeArq, 0x0, 16);
	while((tp_recvfrom(socket_des,nomeArq,20, &cliente))<0);
	printf("Nome do arquivo: %s\n", nomeArq);
	nomeArq[9]='\0';
	FILE *fp = fopen((const char*) nomeArq, "r");
	memset(aux, 0x0, LENGTH);
	printf("1");
	while((bytes_lidos=fread(aux,sizeof(char),LENGTH, fp)) > 0){
		bytes_sendto = tp_sendto(socket_des, aux, bytes_lidos, &cliente);
		bytes_enviados+=bytes_sendto;
		memset(aux, 0x0, LENGTH);
	};

	printf("Conexão encerrada\n");
	fclose(fp);
	close(socket_des);
	free(aux);
	return 0;
}
