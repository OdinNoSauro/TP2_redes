// Cliente
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "tp_socket.h"

#define AMOSTRAS 1

int main (int argc, char *argv[]){

  tp_init();
  so_addr servidor;
	char HOST_SERVIDOR[16];
	strcpy(HOST_SERVIDOR, argv[1]);//host_do_servidor
	int PORTA = atoi(argv[2]); // porta da conexão
	int LENGTH = atoi(argv[4]); // tamanho do buffer
	int mensagens = 0;
	char NOME_ARQUIVO[16];
	memset(NOME_ARQUIVO, 0x0, 16);
	strcpy(NOME_ARQUIVO, argv[3]);
	char *buffer = malloc(LENGTH * sizeof(char));
	int bytes_recebidos = 0;
	int x = 0;
  int socket_des = tp_socket(htons(PORTA)); // descritor do socket
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

  tp_build_addr(&servidor, HOST_SERVIDOR, PORTA);

 	tp_sendto(socket_des, NOME_ARQUIVO, strlen(NOME_ARQUIVO), &servidor);
 	FILE *fp = fopen((const char*) NOME_ARQUIVO, "w+");
	memset(buffer, 0x0, LENGTH);
	while(tp_recvfrom(socket_des, buffer, LENGTH, &servidor) > 0){ // tp_recvfrom: recebe do servidor e guarda no buffer. Retorna a quantidade de chars recebidos							// Continua no loop até não receber mais nada
	 	x = strlen(buffer);
	 	mensagens++;
	 	if(x < LENGTH){
		 	bytes_recebidos += x;
			fwrite(buffer, sizeof(char), x, fp);
		}
		else{
			bytes_recebidos += LENGTH;
			fwrite(buffer, sizeof(char), LENGTH, fp);
		}
		memset(buffer, 0x0, LENGTH);
  }
	printf("Conexão encerrada \n");
	fclose(fp);
	close(socket_des);
	free(buffer);
	return 0;
}
