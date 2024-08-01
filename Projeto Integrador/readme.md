# US 3003

Este documento contém a documentação relativa à US 3003.

## 1. Contexto

Esta *User Story (US)* foi introduzida neste *sprint* para ser desenvolvido um protótipo em C recorrendo às primitivas de 
semáforos e memória partilhada.
Esta *US* faz parte da disciplina de **SCOMP**.

## 2. Requisitos

**US 3003** - As Project Manager, I want the team to "explore" the synchronization problems related to the shyncronization 
of shared boards and design a conceptual solution based on practical evidence

### 2.1 Critérios de aceitação

**CA 1:** This functional part of the system has very specific technical requirements, particularly some concerns about 
synchronization problems.
In fact, several clients will try to concurrently update boards. As such, to explore and study this concurrency scenario 
a ""model"" of a solution must be implemented and evaluated in C, using processes and semaphores.
Specific requirements will be provided in SCOMP.

## 3. Explicação

### 3.1 Padrões Utilizados

O pattern a utilizar neste exercício é o Produtor/Consumidor, em que o processo pai é o produtor e o processo filho o consumidor. 
Este pattern irá fazer uso da exclusão mútua, sincronização de eventos e controlo ao acesso de recursos com uma capacidade limitada.

### 3.2 Pseudo-código do algoritmo
Criar a zona de memória partilhada com os dados necessários a partilhar entre o produtor/consumidor, e outras informações 
relevantes caso necessário.
Criar 3 semáforos:
- Um para garantir que apenas um processo de cada vez acede à zona crítica, quer consumidor quer produtor.
  Este semáforo deve ser iniciado a 1. (mutex)
- Um semáforo para sabermos quantos items se encontram atualmente dentro do buffer circular. Este semáforo
  deve ser inicializado a 0. (full)
- Um semáforo onde é possível saber o espaço disponível no buffer para escrita. Este semáforo deve ser
  iniciado com o valor equivalente ao tamanho máximo de items que podem ser colocados no buffer. (empty)
  Criar um processo filho que representa o comsumidor.

**O processo filho deve:**

Enquanto o processo pai não terminar de escrever no *buffer* ou ainda houver valores no *buffer* para serem consumidos, 
fazer o seguinte:
- Esperar que existam valores a serem lidos do buffer (decrementar semáforo full).
- Realizar a exclusão mútua de acesso à zona crítica (decrementar semáforo mutex).
- Imprimir no ecrã o *id* da *board* acedida no buffer. (Consumir)
- Terminar a exclusão mútua de acesso à zona crítica (incrementar semáforo mutex).
- Avisar que já existe agora uma posição no buffer que já pode ser utilizada para colocar um novo valor,
  pois o que tinha lá já foi consumido. (incrementar semáforo empty)

**O processo pai tem de:**

Escrever no buffer o *id* da *board* criada, em que para cada vez que o fizer, tem de:
- Esperar que exista espaço no buffer para colocarem lá novo valor (decrementar semáforo empty).
- Realizar a exclusão mútua de acesso à zona crítica (decrementar semáforo mutex).
- Inserir o *id* da *board* criada na posição atual disponível do buffer. (Produzir)
- Escrever no ecrã o valor inserido.
- Terminar a exclusão mútua de acesso à zona crítica (incrementar semáforo mutex).
- Indicar que existe um novo item a ser consumido no buffer circular (incrementar semáforo full).

Após terminar de produzir os valores no buffer, deve aguardar que o processo filho termine.
Deve de seguida remover da pasta /dev/shm a memória partilhada e os semáforos criados/utilizados.

**Ambos os processos devem no final:**

Fechar a memória partilhada.
Fechar os semáforos.

## 4. Implementação

### 4.1. Estruturação

O desenvolvimento principal da *US* encontra-se no ficheiro *main.c*. A *struct* usada na memória partilhada está colocada 
no ficheiro *shared_data_type.h*. 

Para uma melhor estruturação do código criou-se duas biblioteca, uma para os semáforos e outra para a memória partilhada
que permitem executar vários métodos específicos contendo as respetivas validações.

O ficheiro *Makefile* permite executar o programa realizado através do comando *Make run*, existe também o comando 
*Make clean* para remover os ficheiros desnecessários que são criados quando o programa é executado.

[Implementação da US](./)

### 4.2. Commits Relevantes

[Listagem dos Commits realizados](https://github.com/Departamento-de-Engenharia-Informatica/sem4pi-22-23-20/issues/33)
