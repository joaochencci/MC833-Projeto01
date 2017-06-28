#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#define LISTEN_PORT 12345
#define MAX_PENDING 5
#define MAX_LINE 256
#define N_CARS 5

#define N 0
#define S 1
#define L 2
#define O 3

#define SECURITY 4
#define ENTERTAINMENT 5
#define CONFORT 6

#define GO 1
#define STOP 0
#define EMERGENCY -1

struct packet {
   int direction;
   int position;
   int speed;
   int size;
   unsigned int port;
   int resolution;
} packet;

struct car {
  int valid;
  struct packet packet;
} car;

struct car cars[N_CARS] = {{0},{0},{0},{0},{0}};

char * printCar(struct packet packet) {
  char * res = (char *) malloc(sizeof(char) * 30);
  sprintf(res, "%d %d %d %d %u", packet.direction, packet.position, packet.speed, packet.size, packet.port);
  return res;
}

struct packet decode(char *rcvMsg, unsigned int port) {
  struct packet p;
  sscanf(rcvMsg, "%d %d %d %d", &p.direction, &p.position, &p.speed, &p.size);
  p.port = port;

  char * str = printCar(p);
  printf("%s \n", str);

  return p;
}

int countValids() {
  int counter = 0, i = 0;
  for(i=0; i<N_CARS; i++) {
    if(cars[i].valid == 1) {
      counter += 1;
    }
  }

  return counter;
}

void addCarPacket(struct packet pkt) {
  int i=0;

  for (i=0; i<N_CARS; i++) {
    if(cars[i].valid == 1) {
      if (cars[i].packet.port == pkt.port)
      {
        struct car car = {1, pkt};
        cars[i] = car;
        break;
      }
    } else {
      struct car car = {1, pkt};
      cars[i] = car;
      break;
    }
  }
}

int colisionAvoidance() {
  if (1 /* array está preenchido com mesmo grupo de movimentos */)
  {
    /* lógica de colisão */

    /* setar os resolutions no array */

    return 1;

  } else {
    
    /* continua aguardando estar preenchido */

    return 0;
  }
}

struct car carByPort() {

  /* TODO: Implementar esse método */

  return cars[0];
}

int main() {
  struct sockaddr_in server, client;
  char buf[MAX_LINE], ip[MAX_LINE];
  unsigned int len;
  int s, new_s, c, read_size, pid;
  int response=1, k=0;

  /* criação da estrutura de dados de endereço */
  //bzero((char *)&server, sizeof(server));

  /* criação de socket passivo */
  // Argumentos 1) Internet domain 2) Stream socket 3) Default protocol (TCP)
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == -1)
  {
    printf("Socket nao foi criado");
  }
  puts("Socket criado");

  /* Associar socket ao descritor */
  // Address family = Internet
  server.sin_family = AF_INET;
  // Porta na conexão
  server.sin_port = htons(LISTEN_PORT);
  // Ip da conexão
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  // Bind da conexão com o socket
  if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0)
  {
    perror("Bind falhou");
    return 1;
  }
  puts("Bind completo");

  /* Criar escuta do socket para aceitar conexões */
  // Inicia conexão do socket com no máximo 5 conexoes simultaneas
  listen(s, MAX_PENDING);

  /* aguardar/aceita conexão, receber e imprimir texto na tela, enviar eco */
  puts("Aguardarndo conexoes...");
  c = sizeof(struct sockaddr_in);

  //aceita conexoes de um client
  while ((new_s = accept(s, (struct sockaddr *)&client, (socklen_t *)&c)))
  {
    // traduz porta e ip
    inet_ntop(AF_INET, &(client.sin_addr), ip, INET_ADDRSTRLEN);
    printf("\nNovo cliente conectado\nIP: %s, Porta: %u\n", ip, ntohs(client.sin_port));

    // cria fork do processo
    if ((pid = fork()) == -1)
    {
      perror("Erro ao criar processo do fork");
      close(new_s);
      continue;
    }

    // processo pai
    if (pid > 0)
    {
      close(new_s);
      continue;
    }

    // processo filho
    //Recebe uma mensagem do client
    while ((read_size = recv(new_s, buf, MAX_LINE, 0)) > 0)
    {
      inet_ntop(AF_INET, &(client.sin_addr), ip, INET_ADDRSTRLEN);
      printf("Nova mensagem recebida de\nIP: %s, Porta: %u\n", ip, ntohs(client.sin_port));

      struct packet pkt = decode(buf, ntohs(client.sin_port));
      addCarPacket(pkt);
      
      do {
        k = colisionAvoidance();
      } while (!k);

      if(response) {
        char * str = (char *) malloc(sizeof(char) * 30);
        // sprintf(str, "%d", carByPort.packet.resolution);
        sprintf(str, "%d", 1);
        write(new_s, str, 1);  
      }

      //clear buffer
      bzero(buf, sizeof(buf));
    }

    if (read_size == 0)
    {
      puts("Cliente desconectado");
      fflush(stdout);
    }
    else if (read_size == -1)
    {
      perror("Recebimento falhou");
    }

    // fecha processo filho
    close(new_s);
  }

  if (new_s < 0)
  {
    perror("Aceite falhou");
    return 1;
  }

  close(new_s);
  close(s);

  return 0;
}
