1. O pattern a utilizar neste exercício é o Leitor/Escritor, em que os leitores e escritores acedem 
a uma zona de memória partilhada, só um leitor ou escritor pode ler ou escrever nessa zona de memória 
partilhada num dado instante. Leitores e escritores são exclusivos e podem estar vários leitores a ler 
ao mesmo tempo. Este pattern irá fazer uso da sincronização de processos com eventos. Neste exercício 
os leitores/escritores são os processos do tipo Ax e Bx.

2. 
Criar na zona de memória partilhada um vetor de números, o números de processos do tipo Ax e Bx, e outras informações 
relevantes caso necessário.
Criar 4 semáforos:
- Um para garantir o acesso exclusivo aos dados compartilhados nos processos do tipo Ax.
- Um para garantir o acesso exclusivo aos dados compartilhados nos processos do tipo Bx.
- Um para garantir que nenhum processo esteja a escrever.
- Um para garantir o acesso exclusivo aos dados compartilhados entre os processos.
Todos os semáforos devem ser inicializados a 1.
Criar vários processos filhos uns que representam os processos do tipo Ax e outros Bx.

*Cada processo filho do tipo Ax tem de**
Decrementar o semáforo de acesso exclusivo aos dados compartilhados nos processos Ax, para garantir que não há processos Ax na secção crítica.
Incrementar o número de processos do tipo Ax.
Se for o primeiro processo Ax, decrementar o semáforo de exclusão mútua para garantir que nenhum processo Bx esteja na zona crítica.
Incrementar o semáforo de acesso exclusivo aos dados compartilhados nos processos Ax, para permitir que outros processos Ax acessem a secção crítica.
Decrementar o semáforo exclusivo para acesso aos dados compartilhados entre os processos.
Dorme 5 segundos.
Escreve e imprime o array de números da região crítica.
Incrementar o semáforo exclusivo para acesso aos dados compartilhados entre os processos.
Decrementar o semáforo de acesso exclusivo aos dados compartilhados nos processos Ax, para garantir que não há processos Ax na secção crítica.
Decrementar o número de processos do tipo Ax.
Se não houver mais processos Ax, incrementar o semáforo de exclusão mútua para permitir que processos Bx possam entrar na zona crítica.
Incrementar o semáforo de acesso exclusivo aos dados compartilhados nos processos Ax, para garantir que não há processos Ax na secção crítica.
Fechar e remover a memória partilhada e os semáforos criados/utilizados.

*Cada processo filho do tipo Bx tem de**
Decrementar o semáforo de acesso exclusivo aos dados compartilhados nos processos Bx, para garantir que não há processos Bx na secção crítica.
Incrementar o número de processos do tipo Bx.
Se for o primeiro processo Bx, decrementar o semáforo de exclusão mútua para garantir que nenhum processo Ax esteja na zona crítica.
Incrementar o semáforo de acesso exclusivo aos dados compartilhados nos processos Bx, para permitir que outros processos Bx acessem a secção crítica.
Decrementar o semáforo exclusivo para acesso aos dados compartilhados entre os processos.
Dorme 5 segundos.
Escreve e imprime o array de números da região crítica.
Incrementar o semáforo exclusivo para acesso aos dados compartilhados entre os processos.
Decrementar o semáforo de acesso exclusivo aos dados compartilhados nos processos Bx, para garantir que não há processos Bx na secção crítica.
Decrementar o número de processos do tipo Bx.
Se não houver mais processos Bx, incrementar o semáforo de exclusão mútua para permitir que processos Ax possam entrar na zona crítica.
Incrementar o semáforo de acesso exclusivo aos dados compartilhados nos processos Bx, para garantir que não há processos Bx na secção crítica.
Fechar e remover a memória partilhada e os semáforos criados/utilizados.

**O processo pai tem de**
Esperar que os processos filhos dos diferentes tipos de processos terminem a sua execução corretamente.
Fechar e remover a memória partilhada e os semáforos criados/utilizados.
