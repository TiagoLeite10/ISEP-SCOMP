1. O pattern a utilizar neste exercício é a exclusão mútua, pois queremos que exista o acesso exclusivo à zona crítica, 
permitindo que apenas um processo de cada vez escreva em memória. (também poderia ser utilizado o pattern Leitor/Escritor)

2. 
Programa A:
Primeiro começamos por criar a zona de memória partilhada com espaço suficiente para armazenar 50 Strings
em que cada uma suporta 80 caracteres. 
Depois criamos um semáforo com valor 1 para controlar o acesso à zona de memória partilhada, e permitir assim 
que seja possível realizar a exclusão mútua, apenas um processo pode estar a aceder à zona crítica para escrita.
Enquanto a zona de memória não tiver todas as 50 strings preenchidas, vamos realizar os seguintes passos:
- Tentar decrementar o semáforo com um tempo limite para o fazer (sem_timedwait()) para que este processo ganhe o 
acesso exclusivo ao semáforo. O tempo limite é de 12 segundos. Caso o tempo limite seja excedido, imprimir no ecrã um aviso.
- Encontrar a primeira zona de memória partilhada em que a String esteja vazia.
- Ao encontrar essa String vazia temos de colocar lá o seguinte texto: "I'm the Father - with PID X",
onde X é o PID do processo atual.
- Gerar um número aleatório entre 1 e 100, que será a percentagem da probabilidade de remover a string acabada de escrever
na zona crítica. A probabilidade de remover deve ser de 30%, sendo que consequentemente existirá 70% de probabilidade da string 
ser mantida.
- Após isso, o processo deve dormir durante um tempo aleatório de entre 1 a 5 segundos.
- Agora libertamos a zona crítica, em que para isso incrementamos 1 ao semáforo (sem_post()), permitindo que outro processo
(ou até este) ganhe o acesso à zona crítica.
- Colocamos o processo a dormir temporariamente novamente com um tempo aleatório de entre 1 a 5 segundos, para evitar que
o mesmo processo esteja constantemente a ganhar o acesso exclusivo à zona crítica.
No final, fechar a zona de memória e fechar o semáforo utilizado.

Programa B:
Este programa deve começar por abrir a zona de memória partilhada no modo de apenas leitura.
Depois deve ler todas as strings não vazias da zona crítica imprimindo-as no ecrã, e no final indicar o número total de strings lidas.
Fechar a zona de memória.
