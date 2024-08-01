1. O pattern a utilizar neste exercício é o Leitor/Escritor, em que os leitores e escritores acedem 
a uma zona de memória partilhada, só um leitor ou escritor pode ler ou escrever nessa zona de memória 
partilhada num dado instante. Leitores e escritores são exclusivos e podem estar vários leitores a ler 
ao mesmo tempo. Os escritores têm prioridade em relação aos leitores. 
Este pattern irá fazer uso da sincronização de processos com eventos.

2.
**O programa principal tem de**
Criar a zona de memória partilhada com o número de leitores e escritores do tipo int, 
e outras informações relevantes caso necessário.
Criar 3 semáforos:
- Um para garantir o acesso exclusivo aos dados compartilhados.
- Um para garantir que nenhum escritor esteja a escrever ou nenhum leitor esteja a ler.
- Um para representar um torniquete. 
Todos os semáforos devem ser inicializados a 1.
Criar vários processos filhos para executar vários escritores ou leitores.
Esperar que o processos filhos terminem a sua execução corretamente.
Fechar e remover a memória partilhada e os semáforos criados/utilizados.

**O programa escritor tem de**
Abrir a zona de memória partilhada e os 3 semáforos criados.
Decrementar o semáforo que representa um torniquete.
Decrementar o semáforo de acesso exclusivo aos dados compartilhados, para garantir que não há leitores na secção crítica.
Incrementar o número de escritores.
Escrever na região de memória compartilhada.
Imprimir o número de escritores e leitores naquele momento.
Decrementar o número de escritores.
Incrementar o semáforo que representa um torniquete.
Incrementar o semáforo de acesso exclusivo aos dados compartilhados, para permitir que leitores acessem a secção crítica.
Fechar a memória partilhada e os semáforos criados/utilizados.

**O programa leitor tem de**
Abrir a zona de memória partilhada e os 3 semáforos criados.
Decrementar o semáforo que representa um torniquete.
Incrementar o semáforo que representa um torniquete.
Decrementar o semáforo de acesso exclusivo aos dados compartilhados.
Incrementar o número de leitores.
Decrementar o semáforo para garantir que nenhum escritor esteja a escrever, se for o primeiro leitor.
Incrementar o semáforo de acesso exclusivo aos dados compartilhados.
Dormir 1 segundo (simulando o tempo de leitura).
Lê e imprime os dados da região de memória compartilhada.
Decrementar o semáforo de acesso exclusivo aos dados compartilhados.
Decrementar o número de leitores.
Incrementar o semáforo para permitir a escrita, se for o último leitor.
Incrementar o semáforo de acesso exclusivo aos dados compartilhados.
Fechar a memória partilhada e os semáforos criados/utilizados.
