1. O pattern a utilizar neste exercício é a exclusão mútua, pois queremos que exista o acesso exclusivo à zona crítica, 
permitindo que apenas um processo de cada vez leia do ficheiro Numbers.txt ou escreva no ficheiro Output.txt evitando que o 
ficheiro fique inconsistente.

2.
Criar 2 semáforos:
- Um para garantir que apenas um processo de cada vez acede à zona crítica de ler do ficheiro Numbers.txt. (sem[0])
- Um para garantir que apenas um processo de cada vez acede à zona crítica de escrever no ficheiro Output.txt. (sem[1])
Ambos os semáforos devem ser iniciado a 1.
Apagar a última versão do ficheiro Output.txt.
Criar os 8 processos filhos.

**Os processos filhos devem**
Ler um número do ficheiro Numbers.txt e escrever esse número juntamento com o seu pid no ficheiro Output.txt, em que 
cada vez que o fizer, tem de:
- Realizar a exclusão mútua de acesso à zona crítica de ler do ficheiro Numbers.txt (decrementar semáforo sem[0]).
- Ler um número de uma linha do ficheiro Numbers.txt.
- Terminar a exclusão mútua de acesso à zona crítica de ler do ficheiro Numbers.txt (incrementar semáforo sem[0]).
- Realizar a exclusão mútua de acesso à zona crítica de escrever no ficheiro Output.txt (decrementar semáforo sem[1]).
- Escrever o número lido e o seu pid no ficheiro Output.txt.
- Terminar a exclusão mútua de acesso à zona crítica escrever no ficheiro Output.txt (incrementar semáforo sem[1]).
No final ambos os semáforos criados/utilizados devem ser fechados.

**O processo pai tem de**
Esperar que todos os processos filhos terminem a sua execução corretamente.
Fechar e remover os semáforos criados/utilizados.
