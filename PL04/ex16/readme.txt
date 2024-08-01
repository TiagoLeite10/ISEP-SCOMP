1. O pattern a utilizar neste exercício é a exclusão mútua e a sincronização de processos com eventos, pois queremos 
a exclusão mútua na análise do 1/5 do vetor entre os vários processos filhos e a sincronização na impressão de uma mensagem.

2. 
Criar na zona de memória partilhada o vetor inicial e final, e outras informações relevantes caso necessário.
Criar 2 semáforos:
- Um para garantir que apenas um processo de cada vez acede à zona crítica.
- Um para ser responsável por imprimir uma mensagem de aviso sempre que um novo valor maior for encontrado no vetor final.
O primeiro semáforo deve ser inicializado a 1 e o restante a 0.
Criar 5 processos filhos e um processo filho adicional.

*Cada processo filho tem de**
Analisar 1/5 do vetor inicial.
Decrementar o semáforo para acesso à zona crítica do vetor inicial.
Realizar a média móvel de 10 valores.
Incrementar o semáforo responsável por imprimir uma mensagem de aviso, caso seja encontrado um novo valor maior no vetor 
final, caso contrário incrementa o semáforo para acesso à zona crítica do vetor inicial.
Fechar e remover a memória partilhada e os semáforos criados/utilizados.

*O processo filho adicional tem de em loop**
Decrementar o semáforo responsável por imprimir uma mensagem de aviso sempre que um novo valor maior for encontrado no vetor final.
Imprimir uma mensagem.
Atualizar o valor da flag.
Incrementar o semáforo para acesso à zona crítica do vetor inicial, caso a flag seja 0.
Fechar e remover a memória partilhada e os semáforos criados/utilizados.

**O processo pai tem de**
Esperar que os 5 processos filhos terminem a sua execução corretamente.
Incrementar o semáforo responsável por imprimir uma mensagem de aviso.
Atualizar o valor da flag para um.
Imprimir o vetor final e o valor máximo encontrado.
Esperar que o processo filho adicional termine a sua execução corretamente.
Fechar e remover a memória partilhada e os semáforos criados/utilizados.
