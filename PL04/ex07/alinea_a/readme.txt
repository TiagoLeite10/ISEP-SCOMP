1. O pattern a utilizar neste exercício é a sincronização de processos com eventos, pois queremos sincronizar 
as ações entre dois processos.

2. 
Criar 2 semáforos:
- Um para garantir a sincronização entre o processo pai e o processo filho.
- Um para garantir a sincronização entre o processo filho e o processo pai.
Ambos os semáforos devem ser inicializados a 0.
Criar um processo filho.

**O processo filho tem de**
Dormir durante um tempo aleatório.
Realizar a ação de comprar batatas fritas.
Incrementar o semáforo de sincronização entre o processo pai e o seu processo.
Decrementar o semáforo de sincronização entre o seu processo e o processo pai.
Realizar a ação de comer e beber.
Fechar os semáforos criados/utilizados.

**O processo pai tem de**
Dormir durante um tempo aleatório.
Realizar a ação de comprar cerveja.
Incrementar o semáforo de sincronização entre o processo filho e o seu processo.
Decrementar o semáforo de sincronização entre o seu processo e o processo filho.
Realizar a ação de comer e beber.
Esperar que o processo filho termine a sua execução corretamente.
Fechar e remover os semáforos criados/utilizados.
