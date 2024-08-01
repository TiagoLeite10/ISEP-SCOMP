1. O pattern a ser utilizado neste exercício é o de Leitor/Escritor, pois queremos que enquanto existir alguém a escrever na memória
partilhada, a leitura esteja bloqueada. Queremos também que a escrita tenha prioridade sobre a leitura. Também é utilizado a exclusão
mútua para a escrita em memória partilhada.

2. 
Passos iniciais comuns aos 3 programas:
Deve ser criada/aberta uma zona de memória partilhada para armazenar a informação de quantos processos leitores se encontram de momento 
a ler da memória partilhada, e quantos clientes se encontram registados (dados funcionais).
Os programas "Consult" e "Consult_All" podem já aqui verificar se existem clientes, e se não existir, podem logo informar isso e terminar.
Deve ser criada/aberta uma zona de memória partilhada suficiente para armazenar até 100 dados de clientes.
Devem ser criados/abertos 3 semáforos:
- Um para garantir que o processo de escrita tem prioridade sobre os processos de leitura que cheguem após o processo de escrita ter 
chegado (sem_torniquete). O semáforo deve ser iniciado com valor 1;
- Um para garantir que apenas um processo leitor de cada vez pode alterar o número de leitores que se encontram a aceder à zona crítica 
(sem_excl). Isto é, para realizar a exclusão mútua. O semáforo deve ser iniciado com valor 1;
- Outro para garantir que unica e exclusivamente um processo de escrita se encontra a aceder e a escrever na zona crítica (sem_no_use). 
O semáforo deve ser iniciado com valor 1.

Programa "Insert"
Pedir os dados do novo cliente.
Deve realizar o sem_wait() do semáforo torniquete, que garante que os processos leitores que cheguem depois disso, fiquem à espera.
Garantir o acesso exclusivo à zona crítica ao decrementar o semáforo no_use.
Escrever na memória partilhada a informação do novo cliente.
Dormir durante 5 segundos para simular que a escrita em memória demora 5 segundos.
Incrementar o semáforo torniquete para permitir que novos processos leitores comecem a iniciar o processo de leitura.
Incrementar o semáforo no_use para permitir os leitores acederem à zona crítica.
Fechar todas as zonas utilizadas de memória partilhada.
Fechar todos os semáforos.

Programa "Consult"
Pede o identificador do cliente a procurar. (**Exclusivo para o programa "Consult"**)

Programa "Consult" e "Consult_All"
Tenta decrementar o semáforo torniquete para garantir que não vai passar à frente de um escritor que esteja prestes ou até já esteja mesmo
a escrever.
Volta a Incrementar o semáforo torniquete, pois já verificou que não existe nenhum escritor à espera de escrever.
Decrementa o semáforo excl para realizar a exclusão mútua de acesso à variável na memória partilhada onde se informa que existe mais um processo 
a ler da memória partilhada.
Incrementa essa variável que indica o número de leitores no momento.
Verifica se o número de leitores é igual a 1, e se for, deve decrementar o semáforo no_use, pois assim bloqueia a escrita dos processos escritores
na memória partilhada. 
Incrementa o semáforo excl para permitir que outro processo de leitura informe que também está prestes a ler da memória partilhada.

Agora, ...
... para o programa "Consult":
- Dormir durante 1 segundo para simular o tempo que demora a realizar esta tarefa.
- Realiza um ciclo que percorre todos os clientes enquanto não encontrar o cliente com o identificador inserido.
- Quando encontrar o cliente desejado, lê da memória os seus dados imprimindo-os no ecrã e termina o ciclo pois já foi encontrado o cliente desejado.
- Se o cliente não for encontrado, informar essa situação imprimindo um aviso no ecrã.

... para o programa "Consult_All":
- Realiza um ciclo para consultar todos os dados válidos dos clientes na memória partilhada e imprime-os no ecrã.
- Antes de imprimir os dados, dormir 1 segundo para simular o tempo que demora a realizar esta tarefa.

Agora para ambos os programas "Consult" e "Consult_All":
Volta a tentar aceder à memória partilhada que contém os dados de quantos leitores estão a ler (exclusão mútua).
Decrementa o número de leitores em 1.
Verifica se era o último leitor (se a variável for igual a 0) e se sim, informa o escritor que já pode escrever se quiser (incrementar o semáforo
no_use).
Liberta a zona de memória partilhada que está a aceder com exclusão mútua. (fim da exclusão mútua)
Fechar todas as zonas utilizadas de memória partilhada.
Fechar todos os semáforos.


Existem algumas "race conditions" que poderiam ocorrer neste tipo de programa, caso não fossem utilizados os semáforos
para sincronizar as operações de leitura e escrita. Estas são:
1. Um programa poderia realizar a consulta de dados enquanto um processo de escrita já poderia estar à espera de conseguir
escrever em memória. Isto iria originar a consulta de dados desatualizados.
2. Se dois processos tentassem escrever ao mesmo tempo em memória, poderia haver informação perdida, pois poderia ser reescrita
pelo outro processo.
3. Caso os escritores não tivessem prioridade, os leitores poderiam passar um dia inteiro a aceder a informação desatualizada.

Estas "race conditions" foram resolvidas ao implementar o pattern de Leitor/Escritor em que permite realizar a sincronização
entre os processos de leitura e os de escrita. De uma forma geral, isto passa por permitir que apenas um processo de cada vez
escreva na memória partilhada ou então vários processos realizem a leitura dos dados ao mesmo tempo. Também há uma particularidade
em que o os escritores têm prioridade sobre os leitores, pois desta forma os leitores irão aceder a dados mais atualizados.
