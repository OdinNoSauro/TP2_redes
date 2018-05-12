#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>

#define AMOSTRAS 100

struct sockaddr_in servidor;
struct sockaddr_in cliente;
int tamanho = sizeof(cliente);


int main (int argc, char *argv[]){

	int PORTA = atoi(argv[1]); // porta da conexão
	int auxiliar = PORTA;
	int LENGTH = atoi(argv[2]); // tamanho do buffer
	char nomeArq[20];
	char buffer [LENGTH];
	struct timeval inicio, fim;
	float media = 0;

	for (int vez = 0; vez < AMOSTRAS; vez++)	{
	PORTA = auxiliar + vez;
	char *aux = malloc(LENGTH*sizeof(char));
	inicio.tv_usec=0;
	fim.tv_usec=0;
	int socket_des; // descritor do socket
	int bytes_enviados = 0;

	double tempo;

	socket_des = socket (AF_INET, SOCK_STREAM, 0);
	if (socket_des == -1){
	perror("socket ");
		exit(1);
	}else
	printf("Socket criado com sucesso\n");

	servidor.sin_family = AF_INET; // Endereço por IP + Porta
	servidor.sin_port = htons(PORTA); // Porta para conexão
	memset(servidor.sin_zero, 0x0, 8); // Zera

	if(bind(socket_des,(struct sockaddr*)&servidor, sizeof(servidor)) == -1){
		perror("bind ");
		exit(1);
	}

	listen (socket_des,1); // Aguarda pelo cliente, Apenas 1 conexão
	int Client 	= accept(socket_des, (struct sockaddr*)&cliente, &tamanho);
	if (Client == -1){
		perror("accept ");
		exit(1);
	}

	printf("Aguardando resposta \n");
	settimeofday(NULL,NULL);

	gettimeofday(&inicio,NULL);
	memset(buffer, 0x0, LENGTH);
	memset(nomeArq, 0x0, 16);
	while(recv(Client, nomeArq, 16, 0)< 0);
	printf("Nome do arquivo: %s\n", nomeArq);
	int x;
	FILE *fp = fopen((const char*) nomeArq, "r");
	memset(aux, 0x0, LENGTH);
	while((x=fread(aux,sizeof(char),LENGTH, fp)) > 0){
		send(Client, aux, x, 0);
		bytes_enviados+=x;
		memset(aux, 0x0, LENGTH);
	};

	printf("Conexão encerrada\n");
	fclose(fp);
	close(socket_des);
	close(Client);
	free(aux);
	gettimeofday(&fim,NULL);
	tempo = (fim.tv_sec - inicio.tv_sec) + (fim.tv_usec - inicio.tv_usec)/1000000.0;
	printf("Buffer = %5i bytes \nBytes enviados: %5i\n",LENGTH, bytes_enviados );
	}
	return 0;
}\
