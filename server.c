#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#define LISTEN_PORT 12345
#define MAX_PENDING 5
#define MAX_LINE 256
#define N_CARS 2

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

struct packet
{
  int direction;
  int position;
  int speed;
  int size;
  unsigned int timestamp;
  unsigned int port;
  int resolution;
} packet;

struct car
{
  int valid;
  struct packet packet;
} car;

char *printCar(struct packet packet)
{
  char *res = (char *)malloc(sizeof(char) * 30);
  sprintf(res, "%d %d %d %d %u", packet.direction, packet.position, packet.speed, packet.size, packet.port);
  return res;
}

struct packet decode(char *rcvMsg, unsigned int port)
{
  struct packet p;

  sscanf(rcvMsg, "%d %d %d %d", &p.direction, &p.position, &p.speed, &p.size);
  p.port = port;

  char *str = printCar(p);
  printf("%s \n", str);

  return p;
}

struct car cars[N_CARS];
int N_GO = 0;
int N_STOP = 0;

void addCarPacket(struct packet pkt)
{
  int i = 0;

  for (i = 0; i < N_CARS; i++)
  {
    if (cars[i].valid == 1)
    {
      if (cars[i].packet.port == pkt.port)
      {
        struct car car = {1, pkt};
        cars[i] = car;
        break;
      }
    }
    else
    {
      struct car car = {1, pkt};
      cars[i] = car;
      break;
    }
  }
}

struct packet moveCar(struct packet packet)
{
  if (packet.direction == N || packet.direction == L)
  {
    packet.position += packet.speed;
    if (packet.position > 11)
    {
      int dif = packet.position - 11;
      packet.position = 11 - dif + 1;

      if (packet.direction == N)
      {
        packet.direction = S;
      }
      else
      {
        packet.direction = O;
      }
    }
  }
  else
  {
    packet.position -= packet.speed;
    if (packet.position < 0)
    {
      packet.position = packet.position * (-1) - 1;

      if (packet.direction == S)
      {
        packet.direction = N;
      }
      else
      {
        packet.direction = L;
      }
    }
  }
  return packet;
}

int detectColision(struct packet car1, struct packet car2)
{
  int c1 = 0, c2 = 0, c3 = 0, c4 = 0, lb = 0, ub = 0;

  if (car1.direction == N || car1.direction == L)
  {
    ub = car1.position;
    lb = car1.position - car1.size + 1;

    if (car1.direction == N)
    {
      if (lb <= 5 && ub >= 5)
      {
        c4++;
      }
      if (lb <= 6 && ub >= 6)
      {
        c2++;
      }
    }
    else
    {
      if (lb <= 5 && ub >= 5)
      {
        c3++;
      }
      if (lb <= 6 && ub >= 6)
      {
        c4++;
      }
    }
  }
  else
  {
    ub = car1.position + car1.size - 1;
    lb = car1.position;

    if (car1.direction == S)
    {
      if (lb <= 5 && ub >= 5)
      {
        c3++;
      }
      if (lb <= 6 && ub >= 6)
      {
        c1++;
      }
    }
    else
    {
      if (lb <= 5 && ub >= 5)
      {
        c2++;
      }
      if (lb <= 6 && ub >= 6)
      {
        c1++;
      }
    }
  }

  if (car2.direction == N || car2.direction == L)
  {
    ub = car2.position;
    lb = car2.position - car2.size + 1;

    if (car2.direction == N)
    {
      if (lb <= 5 || ub >= 5)
      {
        c4++;
      }
      if (lb <= 6 || ub >= 6)
      {
        c2++;
      }
    }
    else
    {
      if (lb <= 5 || ub >= 5)
      {
        c3++;
      }
      if (lb <= 6 || ub >= 6)
      {
        c4++;
      }
    }
  }
  else
  {
    ub = car2.position + car2.size - 1;
    lb = car2.position;

    if (car2.direction == S)
    {
      if (lb <= 5 || ub >= 5)
      {
        c3++;
      }
      if (lb <= 6 || ub >= 6)
      {
        c1++;
      }
    }
    else
    {
      if (lb <= 5 || ub >= 5)
      {
        c2++;
      }
      if (lb <= 6 || ub >= 6)
      {
        c1++;
      }
    }
  }

  printf("c: %d %d %d %d\n", c1, c2, c3, c4);

  if (c1 > 1 || c2 > 1 || c3 > 1 || c4 > 1)
  {
    return 1;
  }
  return 0;
}

int colisionAvoidance(struct packet pkt)
{
  pkt = moveCar(pkt);

  addCarPacket(pkt);

  // verifica sobreposicao de carros
  int i, j;
  for (i = 0; i < N_CARS; i++)
  {
    for (j = 0; j < i; j++)
    {
      if (cars[i].valid && cars[j].valid && detectColision(cars[i].packet, cars[j].packet))
      {
        cars[j].packet.resolution = STOP;
        N_STOP++;
      }
    }
  }

  for (i = 0; i < N_CARS; i++)
  {
    if (cars[i].valid && cars[i].packet.resolution != STOP)
    {
      cars[i].packet.resolution = GO;
      N_GO++;
    }
  }

  for (i = 0; i < N_CARS; i++)
  {
    if (cars[i].valid && cars[i].packet.port == pkt.port)
    {
      return cars[i].packet.resolution;
    }
  }
}

void printResolution()
{
  int i = 0;
  for (i = 0; i < N_CARS; i++)
  {
    if (cars[i].valid)
    {
      printf("Res: %d\n", cars[i].packet.resolution);
    }
  }
}

int main()
{
  struct sockaddr_in server, client;
  char buf[MAX_LINE], ip[MAX_LINE];
  unsigned int len;
  int i, s, new_s, sockfd, c, read_size, cliente_num, maxfd, nready, clientes[FD_SETSIZE];
  fd_set todos_fds, novo_set;
  char so_addr[INET_ADDRSTRLEN];
  int response = 1, k = 0;

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

  maxfd = s;
  cliente_num = -1;
  for (i = 0; i < FD_SETSIZE; i++)
    clientes[i] = -1;

  FD_ZERO(&todos_fds);
  FD_SET(s, &todos_fds);

  //aceita conexoes de um client
  while (1)
  {
    novo_set = todos_fds;
    nready = select(maxfd + 1, &novo_set, NULL, NULL, NULL);
    if (nready < 0)
    {
      perror("select");
      exit(1);
    }
    if (FD_ISSET(s, &novo_set))
    {
      len = sizeof(client);
      if ((new_s = accept(s, (struct sockaddr *)&client, &len)) < 0)
      {
        perror("simplex-talk: accept");
        exit(1);
      }

      // traduz porta e ip
      inet_ntop(AF_INET, &(client.sin_addr), ip, INET_ADDRSTRLEN);
      printf("\nNovo cliente conectado\nIP: %s, Porta: %u\n", ip, ntohs(client.sin_port));

      for (i = 0; i < FD_SETSIZE; i++)
      {
        if (clientes[i] < 0)
        {
          clientes[i] = new_s; //guarda descritor
          break;
        }
      }

      if (i == FD_SETSIZE)
      {
        perror("Numero maximo de clientes atingido.");
        exit(1);
      }
      FD_SET(new_s, &todos_fds); // adiciona novo descritor ao conjunto
      if (new_s > maxfd)
        maxfd = new_s; // para o select
      if (i > cliente_num)
        cliente_num = i; // Ã­ndice mÃ¡ximo no vetor clientes[]
      if (--nready <= 0)
        continue; // nÃ£o existem mais descritores para serem lidos
    }

    for (i = 0; i <= cliente_num; i++)
    { // verificar se hÃ¡ dados em todos os clientes
      if ((sockfd = clientes[i]) < 0)
        continue;
      if (FD_ISSET(sockfd, &novo_set))
      {
        if ((len = recv(sockfd, buf, sizeof(buf), 0)) == 0)
        {
          //conexÃ£o encerrada pelo cliente
          close(sockfd);
          FD_CLR(sockfd, &todos_fds);
          clientes[i] = -1;
        }
        else
        {
          inet_ntop(AF_INET, &(client.sin_addr), ip, INET_ADDRSTRLEN);
          printf("Nova mensagem recebida de\nIP: %s, Porta: %u\n", ip, ntohs(client.sin_port));

          struct packet pkt = decode(buf, ntohs(client.sin_port));

          int resolution = colisionAvoidance(pkt);

          //printResolution();

          printf("\n\n GO: %d, STOP: %d\n\n", N_GO, N_STOP);

          if (response)
          {
            char *str = (char *)malloc(sizeof(char) * 30);
            sprintf(str, "%d", resolution);
            write(new_s, str, 1);
          }
        }
        if (--nready <= 0)
          break; // nÃ£o existem mais descritores para serem lidos
      }
    }
  }
}
