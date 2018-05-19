// Cliente

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include <string.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "tp_socket.h"

#define AMOSTRAS 1
#define TAMANHO_CABECALHO 31

void intParaChar(int inteiro, char* vetor, int inicio, int termino);
int charParaInt(const char* vetor, int inicio, int termino);
int somaDeVerificacao(const char* buffer);
int enviaPacote(int ACK, int flag, int socket_des, so_addr* destino);
int comparaSomas(const char* buffer);

int main (int argc, char *argv[]){
	tp_init();

	char HOST_SERVIDOR[16];
	memset(HOST_SERVIDOR, 0x0, 16);
	strcpy(HOST_SERVIDOR, argv[1]);//host_do_servidor
	int PORTA_SERVIDOR = atoi(argv[2]); // porta da conexão
	char NOME_ARQUIVO[20];
	memset(NOME_ARQUIVO, 0x0, 20);
	strcpy(NOME_ARQUIVO, argv[3]);
	int LENGTH = atoi(argv[4]); // tamanho do buffer

	struct timeval inicio, fim;
	so_addr servidor;

	int PORTA_CLIENTE = 2000;
	int PORTA_SERVIDOR_ORIGINAL = PORTA_SERVIDOR;
	int mensagens = 0;
	int vez;
	float media_t = 0;
	float media_v = 0;
	float desvio = 0;
	float tempo[AMOSTRAS];

	for (vez = 0; vez < AMOSTRAS; vez++){
		char* buffer = malloc(LENGTH*sizeof(char));
		char* letra = malloc(1*sizeof(char));
		PORTA_SERVIDOR = PORTA_SERVIDOR_ORIGINAL + vez;

		inicio.tv_usec = 0;
		fim.tv_usec = 0;

		int socket_des; // descritor do socket
		int bytes_recebidos = 0;
		int x = 0;
		int numero_de_sequencia;
		int ACK = 1;
		int flag = 0;

		settimeofday(NULL, NULL);
		gettimeofday(&inicio, NULL);

		socket_des = tp_socket(PORTA_CLIENTE);

		if (socket_des == -1){
			perror("socket ");
			exit(1);
		}
		else
			printf("Socket criado com sucesso\n");

		tp_build_addr(&servidor, HOST_SERVIDOR, PORTA_SERVIDOR);

		int i = 0;

		//Envia nome do arquivo
		do {
			letra = &NOME_ARQUIVO[i];
			tp_sendto(socket_des, letra, sizeof(char), &servidor);
			i++;
		}while(NOME_ARQUIVO[i]!='\0');
		letra = &NOME_ARQUIVO[i];
		tp_sendto(socket_des, letra, sizeof(char), &servidor);
		
		//Recebe arquivo
		FILE *fp = fopen((const char*) NOME_ARQUIVO, "w+");
		do{
			//1º Recebe pacote do servidor
			memset(buffer, 0x0, LENGTH);
			tp_recvfrom(socket_des, buffer, LENGTH, &servidor);
			x = strlen(buffer);
			bytes_recebidos += x;
			mensagens++;

			//2º Faz soma de verificação e, se correta, verifica número de sequência
			if(comparaSomas(buffer)){
				numero_de_sequencia = charParaInt(buffer, 10, 19);
				
				//3º Se número de sequência igual a ACK, insere no arquivo e envia ACK
				if (ACK == numero_de_sequencia){
					x -= TAMANHO_CABECALHO;
					fwrite(&buffer[TAMANHO_CABECALHO], sizeof(char), x, fp);

					ACK += x;
					flag = charParaInt(buffer, 30, 30);

					enviaPacote(ACK, flag, socket_des, &servidor);
				}
				//4º Se não, reenvia último pacote.
				else{
					enviaPacote(ACK, flag, socket_des, &servidor);
				}
			}
		}while(flag != 1);
		//5º Faz isso até receber flag = 1 no passo 3

		//Encerra e limpa a memória
		printf("Conexão encerrada \n");
		fclose(fp);
		close(socket_des);
		free(buffer);

		//Desempenho
		gettimeofday(&fim, NULL);
		tempo[vez] = (fim.tv_sec - inicio.tv_sec) + (fim.tv_usec - inicio.tv_usec)/1000000.0;
		media_t += tempo[vez];
		printf("Buffer: %5ibytes \nBytes recebidos: %5i",LENGTH, bytes_recebidos );
		printf(" em: %3.6fs\n", tempo[vez]);
		printf("Vazão: %10.2fkbps\n",(float)bytes_recebidos/(1000 * tempo[vez]));
		media_v += bytes_recebidos/(1000 * tempo[vez]);
	}//Fim do for

	//Desempenho
	media_t = media_t/AMOSTRAS;
	for(vez = 0; vez < AMOSTRAS; vez++){
		desvio += pow((tempo[vez] - media_t), 2);
	}
	desvio = sqrt(desvio/AMOSTRAS);
	printf("Número de mensagens: %i\n", mensagens/AMOSTRAS);
	printf("Tempo médio: %fs\n", media_t);
	printf("Desvio padrão do tempo: %fs\n", desvio);
	printf("Vazão média: %10.2fkbps\n", media_v/AMOSTRAS);
	return 0;
}

void intParaChar(int inteiro, char* vetor, int inicio, int termino){
    for(int i = termino; i >= inicio; i--){
    int aux = inteiro%10;
    inteiro = inteiro/10;
        switch (aux){
            case 0:
                vetor[i] = '0';
                break;
            case 1:
                vetor[i] = '1';
                break;
            case 2:
                vetor[i] = '2';
                break;
            case 3:
                vetor[i] = '3';
                break;
            case 4:
                vetor[i] = '4';
                break;
            case 5:
                vetor[i] = '5';
                break;
            case 6:
                vetor[i] = '6';
                break;
            case 7:
                vetor[i] = '7';
                break;
            case 8:
                vetor[i] = '8';
                break;
            case 9:
                vetor[i] = '9';
                break;
        }
    }
}

int charParaInt(const char* vetor, int inicio, int termino){
	int retorno = 0;
	for (int i=inicio; i <= termino; i++){
        retorno *= 10;
		switch (vetor[i]){
            case '1':
                retorno += 1;
                break;
            case '2':
                retorno += 2;
                break;
            case '3':
                retorno += 3;
                break;
            case '4':
                retorno += 4;
                break;
            case '5':
                retorno += 5;
                break;
            case '6':
                retorno += 6;
                break;
            case '7':
                retorno += 7;
                break;
            case '8':
                retorno += 8;
                break;
            case '9':
                retorno += 9;
                break;
        }
	}
	return retorno;
}

int somaDeVerificacao(const char* buffer){
    int soma_buffer = 0;

    for (int i = TAMANHO_CABECALHO; i < strlen(buffer); i++){
        soma_buffer += (int) buffer[i];
        
        //131071(base 10) = 1111.1111.1111.1111(base 2)
        if (soma_buffer > 131071) soma_buffer -= 131071;
    
    }
    
    return soma_buffer;
}

int enviaPacote(int ACK, int flag, int socket_des, so_addr* destino){
	char cabecalho[TAMANHO_CABECALHO];
	/*	cabecalho[0-5] -> Soma de verificação;
	  	cabecalho[6-9] -> Tamanho dados;
		cabecalho[10-19] -> Número de sequência;
		cabecalho[20-29] -> ACK;
		cabecalho[30] -> flag.
	*/
	intParaChar(0, cabecalho, 0, 19);
	intParaChar(ACK, cabecalho, 20, 29);
	intParaChar(flag, cabecalho, 30, 30);

	return tp_sendto(socket_des, cabecalho, TAMANHO_CABECALHO, destino);
}

int comparaSomas(const char* buffer){
	int soma_verificacao_local, soma_verificacao_pacote;
	
	soma_verificacao_local = somaDeVerificacao(buffer);
	soma_verificacao_pacote = charParaInt(buffer, 0, 5);

	if (soma_verificacao_local == soma_verificacao_pacote) return 1;
	else return 0;
}