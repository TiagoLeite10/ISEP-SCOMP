1. O pattern a utilizar neste exercício é a exclusão mútua e a sincronização de processos com eventos, pois queremos 
sincronizar a execução dos dois processos filhos e o processo pai durante 15 execuções.

2. 
Criar 3 semáforos:
- Um para garantir a sincronização entre o 2º processo filho e o 1º processo filho.
- Um para garantir a sincronização entre o 1º processo filho e o pai.
- Um para garantir a sincronização entre o pai e o 2º processo filho.
O primeiro semáforo deve ser inicializado a 1 e os restantes a 0.
Criar os 2 processos filhos.

**O processo filho 1 tem de executar 15 vezes**
Decrementar o semáforo de sincronização entre o 2º processo filho e o seu processo.
Imprimir a sua mensagem.
Incrementar o semáforo de sincronização entre o seu processo e o pai.
No final deve fechar os semáforos criados/utilizados.

**O processo filho 2 tem de executar 15 vezes**
Decrementar o semáforo de sincronização entre o pai e o seu processo.
Imprimir a sua mensagem.
Decrementar o semáforo de sincronização entre o seu processo e o 1º processo filho.
No final deve fechar os semáforos criados/utilizados.

**O processo pai tem de**
Decrementar o semáforo de sincronização entre o 1º processo filho e o seu processo.
Imprimir a sua mensagem.
Incrementar o semáforo de sincronização entre o seu processo e o 2º processo filho.
Esperar que todos os processos filhos terminem a sua execução corretamente.
Fechar e remover os semáforos criados/utilizados.
