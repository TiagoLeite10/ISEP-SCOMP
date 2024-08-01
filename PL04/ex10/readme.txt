1. O pattern a utilizar é o Produtor/Consumidor. Também é utilizada uma barreira e o conceito de sincronização de processos com eventos.

2.
Primeiro temos de criar uma zona de memória partilhada para guardar os dados relativos às informações necessárias para a operação do metro.
Temos também que ter na memória partilhada um array de semáforos para simbolizar cada lugar do metro. (buffer)
Criar 6 semáforos:
- Um para saber o espaço dentro do metro;
- Um para realizar a exclusão mútua de acesso à zona crítica;
- Um para saber os passageiros que entraram no metro;
- Três em que cada um simboliza as 3 portas do metro; (para apenas passar 1 passageiro de cada vez)

O processo principal deve começar por criar 15 filhos que irão de metro até à estação A. 5 deles devem ter como destino a estação A e 10 a estação B.
Depois o processo deve esperar que entrem no metro os 15 filhos. O metro deve de esperar que cada filho incremente um semáforo que avisa que entrou no metro.
Desta forma pode verificar a cada passageiro se já recebeu o número de pessoas que era suposto (15).
Se sim, o metro deve partir para a próxima estação.
Dormir durante 5 segundos, que é o tempo de viagem.
Quando chega à estação A, o metro deve avisar os passageiros que chegou a uma nova estação. Cada passageiro deve verificar se chegou à sua estação.
Este aviso é feito através dos semáforos que estão na memória partilhada, em que na posição que existir um processo, terá de ser avisado.
Antes de partir para a estação B, realiza o mesmo processo que já realizou antes, esperando pelos passageiros entrarem.
Ao chegar à estação B devem ser criados 20 passageiros.
E o processo volta a repetir-se como para a estação A.
Depois volta a partir da estação B, em que após isso ele deve avisar os processos que continuam "dentro do metro" para sairem.

No final, deve sinalizar a memória partilhada e os semáforos para serem removidos.
Deve também fechar a memória partilhada e os semáforos.

**Cada passageiro deve:**
Escolher aleatóriamente uma das 3 portas por onde deve entrar e sair.
Depois deve esperar que exista espaço no metro para entrar, mas durante um tempo limitado.
Se não conseguir entrar nesse tempo, deve avisar a situação.
Se conseguir entrar deve:
- Esperar que consiga entra pela porta que escolheu para entrar. (exclusão mútua)
- Simular a entrar com 0.2 segundos.
- Libertar a porta para que outro passageiro entre ou saia.
- Ganhar acesso exclusívo à zona crítica.
- Indicar que existe mais 1 passageiro no metro.
- Indicar que entrou um novo passageiro.
- Verificar e guardar o seu lugar no metro (posição do array na memória partilhada).
- Libertar a zona crítica.
- Depois deve aguardar que chegue à sua paragem (a cada paragem é avisada pelo semáforo correspondente ao lugar dele, para verificar se é a sua paragem)
- Espera para poder sair pela sua porta.
- Simular a entrar com 0.2 segundos.
- Libertar a porta para que outro passageiro entre ou saia.
- Indica que existe mais um lugar no metro através do incremento do semáforo com essa função.
- Ganha acesso exclusivo à zona crítica.
- Decrementa o número de passageiros e indica que o lugar dele está agora livre.
- Liberta a zona crítica
- Imprime uma mensagem a avisar que saiu do metro.

Fechar a memória partilhada e os semáforos.
