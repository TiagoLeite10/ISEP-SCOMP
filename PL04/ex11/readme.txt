1. O pattern a utilizar neste exercício é a sincronização de processos com eventos, pois queremos sincronizar a 
execução dos processos clientes e o processo vendedor.

2.
**O programa principal tem de**
Criar a zona de memória partilhada com o número do próximo bilhete e número de bilhetes disponíveis do tipo int, 
e outras informações relevantes caso necessário.
Criar 2 semáforos:
- Um para garantir a sincronização entre o processo vendedor e o processo cliente.
- Um para garantir a sincronização entre o processo cliente e o processo vendedor.
Ambos os semáforos devem ser inicializados a 0.
Criar vários processos filhos para executar vários clientes e um processo filho para executar o vendedor
Esperar que o processos filhos terminem a sua execução corretamente.
Fechar e remover a memória partilhada e os semáforos criados/utilizados.

**O programa vendedor tem de em loop enquanto houver bilhetes disponíveis**
Incrementar o semáforo de sincronização entre o processo cliente e o processo vendedor, para ser atendido um cliente.
Decrementar o número de bilhetes disponíveis.
Determinar o número do próximo bilhete vendido.
Decrementar o semáforo de sincronização entre o processo vendedor e o processo cliente, para esperar que o cliente acabe a sua compra.
Fechar a memória partilhada e os semáforos criados/utilizados.

**O programa cliente**
Decrementar o semáforo de sincronização entre o processo cliente e o processo vendedor, para esperar que chegue a vez do cliente ser atendido.
Dormir um tempo aleatório para simular que está a ser atendido.
Imprime o número do seu bilhete.
Incrementar o semáforo de sincronização entre o processo vendedor e o processo cliente, para o vendedor atender outro cliente.
Fechar a memória partilhada e os semáforos criados/utilizados.
