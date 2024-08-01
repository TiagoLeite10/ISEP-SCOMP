1. O pattern a utilizar neste exercício é o Produtor/Consumidor, em que os processos processos filhos
serão os produtores e o processo pai será o consumidor. Este pattern irá fazer uso da exclusão mútua,
sincronização de eventos e controlo ao acesso de recursos com uma capacidade limitada.

2.
Criar a zona de memória partilhada com um buffer circular com capacidade para 10 valores do tipo int,
e outras informações relevantes caso necessário.
Criar 3 semáforos:
- Um para garantir que apenas um processo de cada vez acede à zona crítica, quer consumidor quer produtor.
Este semáforo deve ser iniciado a 1. (excl)
- Um semáforo para sabermos quantos items se encontram atualmente dentro do buffer circular. Este semáforo
deve ser inicializado a 0. (items)
- Um semáforo onde é possível saber o espaço disponível no buffer para escrita. Este semáforo deve ser 
iniciado com o valor equivalente ao tamanho máximo de items que podem ser colocados no buffer. (space)
Criar 2 processos filhos que serão os produtores.

**Os processos filhos devem**
Escrever no buffer 30 valores incrementados, em que para cada vez que o fizer, tem de:
- Esperar que exista espaço no buffer para colocarem lá novo valor (decrementar semáforo space).
- Realizar a exclusão mútua de acesso à zona crítica (decrementar semáforo excl).
- Inserir um valor na posição atual disponível do buffer.
- Escrever no ecrã o valor inserido.
- Terminar a exclusão mútua de acesso à zona crítica (incrementar semáforo excl).
- Indicar que existe um novo item a ser consumido no buffer circular (incrementar semáforo items).
- Dormir durante 1 segundo.

**O processo pai tem de**
Enquanto ambos os processos filhos não terminarem de escrever no buffer ou ainda houver valores no buffer 
para serem consumidos, fazer o seguinte:
- Esperar que existam valores a serem lidos do buffer circular (decrementar semáforo items).
- Realizar a exclusão mútua de acesso à zona crítica (decrementar semáforo excl).
- Imprimir no ecrã o valor atual lido do buffer. (Consumir)
- Terminar a exclusão mútua de acesso à zona crítica (incrementar semáforo excl).
- Avisar que já existe agora uma posição no buffer que já pode ser utilizada para colocar um novo valor, 
pois o que tinha lá já foi consumido. (incrementar semáforo space)
- Dormir durante 2 segundos.

Após terminar de consumir os valores do buffer, deve aguardar que os processos filhos terminem.
Deve de seguida remover da pasta /dev/shm a memória partilhada e os semáforos criados/utilizados.

**Ambos os processos devem no final**
Fechar a memória partilhada.
Fechar os semáforos.
