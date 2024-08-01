1. O pattern a utilizar neste exercício é a Barreira, pois todos os processsos filhos precisam de 
realizar uma ação antes prosseguir. Este pattern irá fazer uso da exclusão mútua e a sincronização 
de processos com eventos

2. 
Criar a zona de memória partilhada com o contador do número de processos na barreira do tipo int, 
e outras informações relevantes caso necessário.
Criar 2 semáforos:
- Um para garantir a exclusão mútua ao contador dos processos que estão à espera na barreira.
- Um para ter os processos bloqueados na barreira.
O primeiro semáforo deve ser inicializado a 1 e o restante a 0.
Criar 6 processos filhos.

*Cada processo filho tem de**
Dormir durante um tempo aleatório.
Realizar a ação de uma compra aleatória.
Aumentar o contador de processos à espera na barreira.
Verificar se todos os processos atingiram a barreira.
Esperar na barreira.
Permitir que o último processo continue.
Realizar a ação de comer e beber.
Fechar a memória partilhada e os semáforos criados/utilizados.

**O processo pai tem de**
Esperar que o processo filho termine a sua execução corretamente.
Fechar e remover a memória partilhada e os semáforos criados/utilizados.
