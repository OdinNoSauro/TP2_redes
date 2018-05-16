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
#define TAMANHO_CABECALHO 10

int timeout = 0;
int socket_des;

void myalarm(int seg){
	alarm(1);
}

void timer_handler(int signum){
	printf("Error: Timeout\n");
	timeout = 1;
	close(socket_des);
}

void settimer(void){
		signal(SIGALRM,timer_handler);
		myalarm(1);
}

void intParaChar(int inteiro, char* vetor, int tamanho){
    for(int i = tamanho-1; i >= 0; i--){
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
    
    intParaChar(soma_buffer_int, soma_buffer_char, 7);
    
    return soma_buffer_char;
}

const char comparaSomas(const char cabecalho[TAMANHO_CABECALHO], const char soma_verificacao[7]){
    char* soma_cabecalho = malloc(sizeof(char)*7);
    strncpy(soma_cabecalho, cabecalho, 6);
    if (!strcmp(soma_verificacao, soma_cabecalho)){
        free(soma_cabecalho);
        return '1';
    }
    else{
        free(soma_cabecalho);
        return '0';
    }
}

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


	char cabecalho[TAMANHO_CABECALHO];
	/*	cabecalho[0-6] -> Soma de verificação;
	  	cabecalho[7-8] -> Tamanho dados + cabeçalho. Expoente na base 2;
		cabecalho[9] -> Recebi sem erros.
	*/

	for (vez = 0; vez < AMOSTRAS; vez++){
		char* buffer = malloc(LENGTH*sizeof(char));
		char* letra = malloc(1*sizeof(char));
		PORTA_SERVIDOR = PORTA_SERVIDOR_ORIGINAL + vez;

		inicio.tv_usec = 0;
		fim.tv_usec = 0;

		int bytes_recebidos = 0;
		int x = 0;

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
			memset(buffer, 0x0, LENGTH);
			
			while(timeout == 0)
				x = tp_recvfrom(socket_des, buffer, LENGTH, &servidor);

			//Extrai o cabeçalho.
			strncpy(cabecalho, buffer, TAMANHO_CABECALHO);
			
			//Faz soma de verificação.
			cabecalho[0] = comparaSomas(cabecalho, somaDeVerificacao(buffer));
			
			//Se incorreta, descarta e espera retransmitir.
		}while(cabecalho[0] == '0');

		myalarm();
		
		
		//Soma correta. Acrescenta dado ao arquivo e pede próximo dado.
			
			
			if(x>0)
				myalarm(1);
			mensagens++;
			if(x < LENGTH){
				bytes_recebidos += x;
				fwrite(buffer, sizeof(char), x, fp);
				printf("%s\n",buffer);
			}
			else{
				bytes_recebidos += LENGTH;
				fwrite(buffer, sizeof(char), LENGTH, fp);
				printf("%s\n",buffer);
			}
			memset(buffer, 0x0, x);
		printf("Conexão encerrada \n");


		//Encerra e limpa a memória
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
