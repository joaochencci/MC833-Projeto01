#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#define LISTEN_PORT 12345
#define MAX_PENDING 5
#define MAX_LINE 256

int main()
{
  struct sockaddr_in server, client;
  char buf[MAX_LINE];
  unsigned int len;
  int s, new_s, c, read_size;

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
    puts("Conexao aceita");

    //Recebe uma mensagem do client
    while ((read_size = recv(new_s, buf, MAX_LINE, 0)) > 0)
    {
      puts("Mensagem recebida: ");
      puts(buf);

      //Send the message back to client
      write(new_s, buf, strlen(buf));

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
