1. Será utilizado o comportamento de semáforos sincronização de processos com eventos. O semáforo utilizado será iniciado com o
valor 1.

2.
Neste exercício será feito o seguinte:
Primeiro temos de tratar os argumentos recebidos para o programa.
Transforma-mos o número de filhos de string para o tipo inteiro. (1º argumento)
Transforma-mos o tempo que cada filho demora a executar em inteiro. (2º argumento)
Criamos o semáforo necessário para realizar a sinalização entre os processos filho e o processo pai. (iniciar o semáforo a 1)
Criamos o número de filhos desejados.

Processos filho:
Dormem durante o tempo passado como argumento.
Realiza o sem_post (somar 1 ao semáforo) do semáforo indicando ao processo pai que mais um filho terminou o seu processamento.

Processo pai:
Fica num while o número de vezes igual aos filhos que tem
Dentro do while realiza o sem_wait para esperar o sem_post do processo filho. Desta forma, o processo filho "indica" que 
terminou de realizar o seu processamento.

No final o processo pai e os processos filhos fecham o semáforo utilizado.
O processo pai dá unlink do semáforo.
