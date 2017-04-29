#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#define SERVER_PORT 12345
#define MAX_LINE 256

int main(int argc, char *argv[])
{
  struct hostent *host_address;
  struct sockaddr_in server;
  char *host;
  char buf[MAX_LINE];
  int s;
  int len;

  /* verificação de argumentos */

  // TODO

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

  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  server.sin_family = AF_INET;
  server.sin_port = htons(SERVER_PORT);

  /* estabelecimento da conexão */
  if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
  {
    perror("Falha no estabelecimento da conexao");
    return 1;
  }
  puts("Conectado\n");

  /* ler e enviar linhas de texto, receber eco */
  while (1)
  {
    printf("Digite uma mensagem para ser enviada ao servidor: ");
    scanf("%s", buf);

    //Envia a mensagem
    if (send(s, buf, strlen(buf), 0) < 0)
    {
      puts("Envio falhou");
      return 1;
    }

    //Recebe a resposta do servidor
    if (recv(s, buf, MAX_LINE, 0) < 0)
    {
      puts("Recebimento falhou");
      break;
    }
    puts("Resposta do servidor: ");
    puts(buf);
  }

  close(s);
  return 0;
}
