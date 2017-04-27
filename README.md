<h1>Projeto 1</h1> 
<h1>Implementação de cliente-servidor simples usando sockets.</h2>

<h2>Bons links:</h2>

- http://www.programminglogic.com/example-of-client-server-program-in-c-using-sockets-and-tcp/
- http://www.binarytides.com/server-client-example-c-sockets-linux/

<h2>Enunciado:</h2>

- Linguagem: C
- Data de entrega: 4 de maio.
- Conteúdo da entrega: código fonte com a implementação especificada e relatório descrevendo a implementação, as funções utilizadas e mostrando os testes realizados para validar a implementação.
- Em dupla.
- Forma de entrega: arquivo comprimido (tar.gz, zip.) via classroom.google.com

<h3>Tarefa</h3>

- Implemente um cliente e um servidor TCP usando sockets em C. Você pode basear-se nos esqueletos abaixo.
- O cliente que se conecta ao servidor deve poder enviar mensagens de texto, que são reproduzidas na janela do servidor. O servidor responde com a mesma mensagem, que é então exibida na janela do cliente (eco). O envio e recebimento das mensagens podem ser feitos linha a linha.
- O cliente recebe como parâmetro, na linha de comando, o hostname do servidor.
- Seus programas devem emitir mensagens de erro quando necessário (número incorreto de argumentos, erro na resolução de nome, erro na alocação do socket, erro nas chamadas accept, connect, etc).

<h3>Funções úteis:</h3>

- gethostbyname
- socket
- connect
- bind
- listen
- accept
- send
- recv
- htons
- bcopy
- close
