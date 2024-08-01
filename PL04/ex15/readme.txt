**NOTA:** As soluções desenvolvidas no exercício A e B, utilizam um buffer circular para colocar os clientes por ordem de chegada. Esta solução apenas garante
a ordem de chegada para o número de clientes igual ao número máximo de espaço no buffer. Caso existam clientes em excesso e ultrapassem
o tamanho do buffer circular, a ordem para aqueles que chegam depois da lotação não é assegurada. Desta forma, a solução a utilizar caso
sejam esperados mais clientes do que o tamanho atual do buffer, é aumentar esse mesmo tamanho.

Exercício a)
1. O pattern a utilizar é o Produtor/Consumidor, de forma a controlar os clientes na fila de espera. É também utilizada a técnica de sincronização de processos com eventos.

2.
Criar uma zona de memória partilhada para controlar o acesso dos clientes em fila de espera ao bar, em que deve ser 
utilizado um array de semáforos, e devemos também guardar a posição da fila onde o próximo cliente deve entrar, e ainda 
a posição do próximo cliente a entrar no bar.
Criar 4 semáforos:
- Um para saber o espaço existente na fila
- Um para saber o espaço existente no bar.
- Um para fazer a exclusão mútua;
- Um para saber a quantidade de clientes que estão na fila à espera.
Agora como na zona partilhada tem de existir um array de semáforos, devemos inicializar todos esses semáforos. (lembrar: cada um tem o seu nome, para não abrir todos o mesmo semáforo)
Criar uma quantidade de processos filhos que vão representar os clientes.

**Processo pai (como se fosse um segurança do bar):**
- Esperar que exista clientes na fila. Aqui pode ser utilizado o mecanismo de colocar o tempo limite de espera, pois desta forma podemos fechar o bar automáticamente caso não haja mais clientes.
- Indicar o número de clientes que se encontram dentro do bar atualmente.
- Esperar que exista espaço dentro do bar.
- Informar que um cliente vai entrar no bar.
- Sinalizar o cliente que se encontra em primeiro na fila de espera, informando assim que pode entrar no bar. (Incrementar o semáforo que lhe pertence)
- Tratar a variável que indica o índice da filo do próximo cliente a entrar no bar. (Não esquecer que é um buffer circular e se índice == buffer_size, então índice = 0)
- Dormir durante um segundo para garantir que o cliente que foi "sinalizado" entra sem ser atropelado pelo próximo cliente que também poderá já poder entrar.

Após não existirem mais clientes na fila de espera por um determinado tempo, fazer:
- Esperar que todos os clientes saiam do bar (esperar que todos os processos filhos terminem).
- Sinalizar a zona de memória partilhada utilizada para remoção.
- Sinalizar todos os semáforos criados/utilizados para remoção.

**Processos filho (clientes):**
- Espera que exista espaço na fila.
- Ganhar acesso exclusívo à zona crítica.
- Guardar a posição que se encontra livre a seguir ao último cliente na fila.
- Atualizar a posição que o próximo cliente deve entrar. (cuidado com index == buffer_size...)
- Liberta a zona crítica.
- Aumenta o semáforo que sabe o número de clientes que se encontram em fila de espera.
- Informa que entrou na fila de espera mostrando o PID do processo.
- Espera que chegue a sua vez de entrar no bar (espera pelo incremento do semáforo na posição que ele se encontra no buffer).
- Informa que entrou agora no bar e indica a posição que estava no buffer.
- Incrementa o semáforo que sabe o espaço existente na fila de espera, pois já entrou no bar.
- Dorme durante um tempo aleatório de 1 a 20 segundos para simular o tempo que esteve dentro do bar.
- Informa que vai sair do bar e indica o tempo total que lá esteve.
- Incrementa o semáforo que sabe o espaço total existente dentro do bar, permitindo assim que entre outro cliente.

Todos os processos no final devem de:
Fechar todos os semáforos utilizados
Fechar a zona de memória partilhada utilizada.

Exercício b)
Criar uma zona de memória partilhada para controlar o acesso dos clientes em filas de espera ao bar. Para cada tipo de cliente deve existir uma fila de espera.
Para cada fila deve ser ainda possível saber a posição da fila onde o próximo cliente deve entrar para espera, e ainda saber a posição do próximo cliente a entrar no bar.
Criar 9 semáforos:
- Um para saber o espaço existente na fila de clientes normais.
- Um para saber o espaço existente na fila de clientes especiais.
- Um para saber o espaço existente na fila de clientes vips.
- Um para fazer a exclusão mútua;
- Um para saber a quantidade de clientes que estão à espera de entrar no bar em qualquer uma das filas.
- Um para saber o espaço existente no bar.
- Um para saber quantos clientes normais estão à espera de entrar no bar.
- Um para saber quantos clientes especiais estão à espera de entrar no bar.
- Um para saber quantos clientes vips estão à espera de entrar no bar.
Agora como na zona partilhada tem de existir 3 arrays de semáforos (um array para cada tipo de fila), devemos inicializar todos esses semáforos. (lembrar: cada um tem o seu nome, para não abrir todos o mesmo semáforo)
Criar uma quantidade de processos filhos que vão representar os clientes.

**Processo pai (como se fosse um segurança do bar):**
- Esperar que exista clientes na fila. Manter o mesmo formato do exercício A.
- Informar o número de clientes que se encontram dentro do bar atualmente.
- Esperar que exista espaço dentro do bar.
- Informar que pode entrar mais um cliente no bar.
- Verificar por ordem o tipo de cliente que possa ser (sem_trywait, primeiro verificar quantidade de vips na fila, depois especiais e por fim normais).
- Sinalizar o cliente próximo cliente (tomando em consideração o tipo de cliente), informando-lhe que pode entrar no bar. (Incrementar o semáforo que lhe pertence)
- Tratar a variável que indica o índice da fila do próximo cliente a entrar no bar. (Não esquecer que é um buffer circular e se índice == buffer_size, então índice = 0)
- Dormir durante um segundo para garantir que o cliente que foi "sinalizado" entra sem ser atropelado pelo próximo cliente que também poderá já poder entrar.

Após não existirem mais clientes na fila de espera por um determinado tempo, fazer:
- Esperar que todos os clientes saiam do bar (esperar que todos os processos filhos terminem).
- Sinalizar a zona de memória partilhada utilizada para remoção.
- Sinalizar todos os semáforos criados/utilizados para remoção.

**Processos filho (clientes):**
- De forma aleatória, deve escolher o seu tipo de cliente (normal, especial ou vip).
- Espera que exista espaço na fila do seu tipo de cliente.
- Ganhar acesso exclusívo à zona crítica.
- Guardar a posição que se encontra livre a seguir ao último cliente na sua fila.
- Atualizar a posição que o próximo cliente deve entrar nessa mesma fila. (cuidado com index == buffer_size...)
- Liberta a zona crítica.
- Aumenta o semáforo que sabe o número de clientes que se encontram naquela fila de espera específica.
- Aumenta o semáforo que sabe o número geral de clientes a tentar entrar no bar.
- Informa que entrou na fila de espera mostrando o PID do processo e o seu tipo de cliente.
- Espera que chegue a sua vez de entrar no bar (espera pelo incremento do semáforo na posição que ele se encontra no buffer daquele tipo de cliente).
- Informa que entrou agora no bar (imprimir pid e tipo cliente) e indica a posição que estava no buffer daquele tipo de cliente.
- Incrementa o semáforo que sabe o espaço existente na fila de espera daquele tipo de cliente, pois ele já entrou no bar.
- Dorme durante um tempo aleatório de 1 a 20 segundos para simular o tempo que esteve dentro do bar.
- Informa que vai sair do bar e indica o tempo total que lá esteve.
- Incrementa o semáforo que sabe o espaço total existente dentro do bar, permitindo assim que entre outro cliente.
- Informa que já saiu do bar, indicando o seu pid, o seu tipo de cliente e o número de clientes que ficaram lá.

Todos os processos no final devem de:
Fechar todos os semáforos utilizados
Fechar a zona de memória partilhada utilizada.
