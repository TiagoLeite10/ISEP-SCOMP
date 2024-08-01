1. O pattern a utilizar neste exercício é a sincronização de processos com eventos e a exclusão mútua, pois queremos que exista o acesso 
exclusivo à zona crítica, permitindo que apenas um processo de cada vez leia do ficheiro Numbers.txt e escreva no ficheiro Output.txt 
evitando que o ficheiro fique inconsistente.

2.
Criar 8 semáforos:
- Um para cada processo filho para garantir que apenas um processo de cada vez acede à zona crítica (Ler do ficheiro Numbers.txt e 
escrever no ficheiro Output.txt pela ordem da sua geração, ou seja, do primeiro ao oitavo processo). 
O semáforo relativo ao primeiro processo deve ser iniciado a 1 e os restantes a 0.
Apagar a última versão do ficheiro Output.txt.
Criar os 8 processos filhos.

**Os processos filhos devem**
Ler os 200 números do ficheiro Numbers.txt e escrever esses números juntamento com o seu pid no ficheiro Output.txt, em que 
cada vez que o fizer, tem de:
- Realizar a exclusão mútua de acesso à zona crítica (decrementar o semáforo associado ao seu processo).
- Ler os números do ficheiro Numbers.txt.
- Escrever os números lidos e o seu pid no ficheiro Output.txt.
- Terminar a exclusão mútua de acesso à zona crítica (incrementar semáforo associado ao próximo processo).
No final todos os semáforos criados/utilizados devem ser fechados.

**O processo pai tem de**
Esperar que todos os processos filhos terminem a sua execução corretamente.
Fechar e remover os semáforos criados/utilizados.
