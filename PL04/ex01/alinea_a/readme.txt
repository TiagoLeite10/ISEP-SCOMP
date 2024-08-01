1. O pattern a utilizar neste exercício é a exclusão mútua, pois queremos que exista o acesso exclusivo à zona crítica, 
permitindo que apenas um processo de cada vez leia do ficheiro Numbers.txt e escreva no ficheiro Output.txt evitando que o 
ficheiro fique inconsistente.

2. 
Criar 1 semáforo para garantir que apenas um processo de cada vez acede à zona crítica (Ler do ficheiro Numbers.txt e 
escrever no ficheiro Output.txt). Este semáforo deve ser iniciado a 1.
Apagar a última versão do ficheiro Output.txt.
Criar os 8 processos filhos.

**Os processos filhos devem**
Ler um número do ficheiro Numbers.txt e escrever esse número juntamento com o seu pid no ficheiro Output.txt, em que 
cada vez que o fizer, tem de:
- Realizar a exclusão mútua de acesso à zona crítica (decrementar semáforo).
- Ler um número de uma linha do ficheiro Numbers.txt.
- Escrever o número lido e o seu pid no ficheiro Output.txt.
- Terminar a exclusão mútua de acesso à zona crítica (incrementar semáforo).
No final o semáforo criado/utilizado deve ser fechado.

**O processo pai tem de**
Esperar que todos os processos filhos terminem a sua execução corretamente.
Fechar e remover o semáforo criado/utilizado.
