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

#define AMOSTRAS 100

struct sockaddr_in cliente;


int main (int argc, char *argv[]){

	char HOST_SERVIDOR[16];
	strcpy(HOST_SERVIDOR, argv[1]) ;//host_do_servidor
	int PORTA = atoi(argv[2]); // porta da conexão
	int auxiliar = PORTA;
	int LENGTH = atoi(argv[4]); // tamanho do buffer
	int mensagens = 0;
	char NOME_ARQUIVO[16];
	memset(NOME_ARQUIVO, 0x0, 16);
	strcpy(NOME_ARQUIVO, argv[3]);
	struct timeval inicio, fim;
	int vez;
	float media_t = 0;
	float media_v = 0;
	float desvio = 0;
	float tempo[AMOSTRAS];

	for (vez = 0;vez<AMOSTRAS;vez++){
	char *buffer = malloc(LENGTH*sizeof(char));
	PORTA = auxiliar + vez;
	inicio.tv_usec=0;
	fim.tv_usec=0;
	int socket_des; // descritor do socket
	int bytes_recebidos = 0;
	int x = 0;
	settimeofday(NULL,NULL);
	gettimeofday(&inicio,NULL);
	socket_des = socket (AF_INET, SOCK_STREAM, 0);
	if (socket_des == -1){
		perror("socket ");
		exit(1);
	}else
	printf("Socket criado com sucesso\n");

	cliente.sin_family = AF_INET; // Endereço por IP + Porta
	cliente.sin_port = htons(PORTA); // Porta para conexão
	cliente.sin_addr.s_addr = inet_addr(HOST_SERVIDOR);
	memset(cliente.sin_zero, 0x0, 8); // Zera

	if (connect(socket_des, (struct sockaddr*)&cliente, sizeof(cliente)) == -1){
		perror("connect");
		exit(1);
	}

 	send(socket_des, NOME_ARQUIVO, strlen(NOME_ARQUIVO), 0);
 	FILE *fp = fopen((const char*) NOME_ARQUIVO, "w+");
	memset(buffer, 0x0, LENGTH);
	while(recv(socket_des, buffer, LENGTH, 0) > 0){ // recv: recebe do servidor e guarda no buffer. Retorna a quantidade de chars recebidos							// Continua no loop até não receber mais nada
	 	x = strlen(buffer);
	 	mensagens++;
	 	if(x<LENGTH){
		 	bytes_recebidos+=x;
			fwrite(buffer, sizeof(char), x, fp);
		}
		else{
			bytes_recebidos+=LENGTH;
			fwrite(buffer, sizeof(char), LENGTH, fp);
		}
		memset(buffer, 0x0, LENGTH);
  	}
	printf("Conexão encerrada \n");
	fclose(fp);
	close(socket_des);
	free(buffer);
	gettimeofday(&fim,NULL);
	tempo[vez] = (fim.tv_sec - inicio.tv_sec) + (fim.tv_usec - inicio.tv_usec)/1000000.0;
	media_t+=tempo[vez];
	printf("Buffer:%5i bytes \nBytes recebidos: %5i",LENGTH, bytes_recebidos );
	printf(" em: %3.6fs\n", tempo[vez]);
	printf("Vazão: %10.2fkbps\n",(float)bytes_recebidos/(1000*tempo[vez]));
	media_v += bytes_recebidos/(1000*tempo[vez]);
	}
	media_t=media_t/AMOSTRAS;
	for(vez=0;vez<AMOSTRAS;vez++){
		desvio+=pow((tempo[vez]-media_t),2);
	}
	desvio=sqrt(desvio/AMOSTRAS);
	printf("Número de mensagens: %i\n",mensagens/AMOSTRAS);
	printf("Tempo médio: %fs\n",media_t);
	printf("Desvio padrão do tempo: %fs\n",desvio);
	printf("Vazão média: %10.2fkbps\n",media_v/AMOSTRAS);
	return 0;
}
