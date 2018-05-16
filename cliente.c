// Cliente

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#include <sys/time.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "tp_socket.h"

#define AMOSTRAS 1
#define TAMANHO_CABECALHO 31

int timeout = 0;
int socket_des;

void myalarm(int seg);
void timer_handler(int signum);
void settimer(void);
void intParaChar(int inteiro, char* vetor, int inicio, int termino);
const char* somaDeVerificacao(const char* buffer);
int comparaSomas(const char* buffer);
void enviaPacote(int somaDeVerificacao, int numero_de_sequencia, int ACK, char* buffer, int socket_des, so_addr* destino);

int main (int argc, char *argv[]){
	tp_init();
	settimer();

	char HOST_SERVIDOR[16];
	strcpy(HOST_SERVIDOR, argv[1]);//host_do_servidor
	int PORTA_SERVIDOR = atoi(argv[2]); // porta da conexão
	char NOME_ARQUIVO[20];
	memset(NOME_ARQUIVO, 0x0, 20);
	strcpy(NOME_ARQUIVO, argv[3]);
	int LENGTH = atoi(argv[4]); // tamanho do buffer

	struct timeval inicio, fim;
	so_addr servidor;

	int PORTA_CLIENTE = PORTA_SERVIDOR+1;
	int PORTA_SERVIDOR_ORIGINAL = PORTA_SERVIDOR;
	int mensagens = 0;
	int vez;
	float media_t = 0;
	float media_v = 0;
	float desvio = 0;
	float tempo[AMOSTRAS];

	for (vez = 0; vez < AMOSTRAS; vez++){
		char* buffer = malloc(LENGTH*sizeof(char));
		memset(buffer, 0x0, LENGTH);
		
		char* letra = malloc(1*sizeof(char));
		PORTA_SERVIDOR = PORTA_SERVIDOR_ORIGINAL + vez;

		inicio.tv_usec = 0;
		fim.tv_usec = 0;

		int bytes_recebidos = 0;
		int ACK = 0;
		int x = 0;
		int quantidade_dados;
		
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
		do{
			memset(buffer, 0x0, sizeof(buffer));
			//Enquanto o timeout não expirar e não chegar nada, espera o pacote
			while((timeout == 0)&&(x = 0)){
				x = tp_recvfrom(socket_des, buffer, LENGTH, &servidor);
			}
			//Se o timeout expirar, reenvia o pacote anterior
			if (timeout != 0){
				enviaPacote(0, 0, ACK, buffer, socket_des, &servidor);
			}
			else{
				mensagens++;
				bytes_recebidos += x;
				int somasIguais = comparaSomas(buffer);

				//Se a soma estiver errada, reenvia o pacote anterior	
				if (!somasIguais){
					memset(buffer, 0x0, sizeof(buffer));
					enviaPacote(0, 0, ACK, buffer, socket_des, &servidor);
				}

				//Se a soma estiver certa, atualiza ACK, insere dados no arquivo e envia pacote
				else{
					quantidade_dados = x - TAMANHO_CABECALHO;
					ACK += quantidade_dados;
					
					FILE *fp = fopen((const char*) NOME_ARQUIVO, "w+");
					fwrite(&buffer[TAMANHO_CABECALHO], sizeof(char), quantidade_dados, fp);
					printf("%s\n",buffer);
					fclose(fp);		
					
					memset(buffer, 0x0, sizeof(buffer));
					enviaPacote(0, 0, ACK, buffer, socket_des, &servidor);
				}	
			}
			myalarm(1);
		}while(quantidade_dados);
		//Enquanto quantidade de dados for diferente de 0.

		printf("Comunicação encerrada \n");	
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


void myalarm(int seg){
	alarm(1);
}

void timer_handler(int signum){
	printf("Error: Timeout\n");
	timeout = 1;
}

void settimer(void){
		signal(SIGALRM,timer_handler);
		myalarm(1);
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

const char* somaDeVerificacao(const char* buffer){
    int soma_buffer_int = 0;
    char* soma_buffer_char = malloc(sizeof(char)*7);

    for (int i = TAMANHO_CABECALHO; i < strlen(buffer); i++){
        soma_buffer_int += (int) buffer[i];
        
        //131071(base 10) = 1111.1111.1111.1111(base 2)
        if (soma_buffer_int > 131071) soma_buffer_int -= 131071;
    
    }
    
    intParaChar(soma_buffer_int, soma_buffer_char, 0, 7);
    
    return soma_buffer_char;
}

int comparaSomas(const char* buffer){
	char* soma_verificacao = malloc(sizeof(char)*7);
	soma_verificacao = somaDeVerificacao(buffer);

    char* soma_cabecalho = malloc(sizeof(char)*7);
	strncpy(soma_cabecalho, buffer, 6);

    if (!strcmp(soma_verificacao, soma_cabecalho)){
        free(soma_verificacao);
		free(soma_cabecalho);
        return 1;
    }
    else{
		free(soma_verificacao);
        free(soma_cabecalho);
        return 0;
    }
}

void enviaPacote(int somaDeVerificacao, int numero_de_sequencia, int ACK, char* buffer, int socket_des, so_addr* destino){
	char cabecalho[TAMANHO_CABECALHO];
	/*	cabecalho[0-6] -> Soma de verificação;
	  	cabecalho[7-10] -> Tamanho dados;
		cabecalho[11-20] -> Número de sequência;
		cabecalho[21-30] -> ACK.
	*/
	intParaChar(somaDeVerificacao, cabecalho, 0, 6);
	intParaChar(strlen(buffer), cabecalho, 7, 10);
	intParaChar(numero_de_sequencia, cabecalho, 11, 20);
	intParaChar(ACK, cabecalho, 21, 30);
	
	memcpy(buffer, cabecalho, TAMANHO_CABECALHO-1);

	tp_sendto(socket_des, buffer, sizeof(buffer), destino);
}