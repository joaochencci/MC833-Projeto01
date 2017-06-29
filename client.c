#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>
#include <arpa/inet.h>

#define SERVER_PORT 12345
#define MAX_LINE 256

#define N 0
#define S 1
#define L 2
#define O 3

#define SECURITY 4
#define ENTERTAINMENT 5
#define CONFORT 6

#define GO 1
#define STOP 2
#define EMERGENCY 3

struct packet {
   int direction;
   int position;
   int speed;
   int size;
   int port;
   int resolution;
} packet;

char * printCar(struct packet packet) {
  char * res = (char *) malloc(sizeof(char) * 30);
  sprintf(res, "%d %d %d %d", packet.direction, packet.position, packet.speed, packet.size);
  return res;
}

struct packet initializeCar() {
  time_t t;
  srand((unsigned) time(&t));

  int direction = rand() % 4;
  int position = (direction%2 == 0) ? 11 : 0;
  int speed = (rand() % 3) + 1;
  int size = (rand() % 3) + 1;

  struct packet pkt = {direction, position, speed, size};
  char * inputStr = printCar(pkt);
  printf("Initial State: %s \n", inputStr);
  return pkt;
}

struct packet moveCar(int controlSignal, struct packet packet) {
  if (controlSignal == GO) {

    if (packet.direction == N || packet.direction == L) {
      packet.position += packet.speed;
      if (packet.position > 11) {
        int dif = packet.position - 11;
        packet.position = 11 - dif + 1;

        if (packet.direction == N)
        {
          packet.direction = S;
        } else {
          packet.direction = O;
        }
      }
    } else {
      packet.position -= packet.speed;
      if (packet.position < 0) {
        packet.position = packet.position*(-1) - 1;

        if (packet.direction == S)
        {
          packet.direction = N;
        } else {
          packet.direction = L;
        }
      }
    }

    if (packet.speed < 3) {
      packet.speed += 1;
    }
    
  } else {
    packet.speed = 0;
  }

  return packet;
}

int main(int argc, char *argv[])
{
  struct hostent *host_address;
  struct sockaddr_in server;
  char *host;
  char buf[MAX_LINE], ip[MAX_LINE];
  int s;
  int len;

  /* verificação de argumentos */
  if (argc != 2)
  {
    puts("Numero invalido de argumentos");
    puts("Passe o endereco do servidor socket a ser conectado");
    return 1;
  }

  /* tradução de nome para endereço IP */
  // TODO

  /* criação da estrutura de dados de endereço */
  //bzero((char *)&socket_address, sizeof(socket_address));

  /* criação de socket ativo*/
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == -1)
  {
    printf("Socket nao foi criado");
  }
  puts("Socket criado");

  server.sin_addr.s_addr = inet_addr(argv[1]);
  server.sin_family = AF_INET;
  server.sin_port = htons(SERVER_PORT);

  /* estabelecimento da conexão */
  if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
  {
    perror("Falha no estabelecimento da conexao");
    return 1;
  }

  inet_ntop(AF_INET, &(server.sin_addr), ip, INET_ADDRSTRLEN);
  printf("\nConectado em\nIP: %s Porta: %u\n", ip, ntohs(server.sin_port));

  printf("O carro inicializado é: \n");
  struct packet pkt = initializeCar();

  /* ler e enviar linhas de texto, receber eco */
  while (1)
  {
    // printf("O carro inicializado é: \n");
    // fgets(buf, MAX_LINE, stdin);
    char * inputStr = printCar(pkt);

    //Envia a mensagem
    if (send(s, inputStr, strlen(inputStr), 0) < 0)
    {
      puts("Envio falhou");
      return 1;
    }

    printf("State: %s \n", inputStr);

    //Recebe a resposta do servidor
    if (recv(s, buf, MAX_LINE, 0) < 0)
    {
      puts("Recebimento falhou");
      break;
    }
    puts("Resposta do servidor: ");
    // puts(buf);
    int controlSignal;
    sscanf(buf, "%d", &controlSignal);
    pkt = moveCar(controlSignal, pkt);
  }

  close(s);
  return 0;
}
