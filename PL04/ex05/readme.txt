1. O pattern a utilizar neste exercício é a exclusão mútua e a sincronização de processos, pois queremos 
sincronizar a escrita de palavras entre 3 processos filhos, sendo que apenas um processo escreve de cada vez.

2. 
Criar 3 semáforos:
- Um para garantir a sincronização entre o 3º processo filho e o 1º processo filho.
- Um para garantir a sincronização entre o 1º processo filho e o 2º processo filho.
- Um para garantir a sincronização entre o 2º processo filho e o 3º processo filho.
O primeiro semáforo deve ser inicializado a 1 e os restantes a 0.
Criar os 3 processos filhos.

**O processo filho 1 tem de**
Decrementar o semáforo de sincronização entre o 3º processo filho e o seu processo.
Imprimir a sua palavra.
Incrementar o semáforo de sincronização entre o seu processo e o 2º processo filho.
No final deve fechar os semáforos criados/utilizados.

**O processo filho 2 tem de**
Decrementar o semáforo de sincronização entre o 1º processo filho e o seu processo.
Imprimir a sua palavra.
Incrementar o semáforo de sincronização entre o seu processo e o 3º processo filho.
No final deve fechar os semáforos criados/utilizados.

**O processo filho 3 tem de**
Decrementar o semáforo de sincronização entre o 2º processo filho e o seu processo.
Imprimir a sua palavra.
Incrementar o semáforo de sincronização entre o seu processo e o 1º processo filho.
No final deve fechar os semáforos criados/utilizados.

Como cada processo filho tem de imprimir duas palavras (uma de cada vez) realiza estas instruções num loop.

**O processo pai tem de**
Esperar que todos os processos filhos terminem a sua execução corretamente.
Fechar e remover os semáforos criados/utilizados.
