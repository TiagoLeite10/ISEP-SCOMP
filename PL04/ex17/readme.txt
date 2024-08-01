1. O pattern utilizada neste exercício foi o Produtor/Consumidor, com a utilização de buffer circular.

2. 
Criar uma estrutura para receber os dados de um sensor. Estes dados incluem o valor lido, se o alarme entrou no modo alarme
ou se saiu do modo alarme.
Devem ser criadas as zonas de memória partilhada necessárias. Uma com espaço para 5 estruturas que contenham dados da leitura
de um sensor e outra zona para guardar o índice de escrita na zona anterior, para que os processos filhos saibam entre sí o 
local onde devem escrever na memória.

Agora devem ser criados 3 semáforos:
- Um para controlar o acesso exclusívo à zona de memória;
- Um para indicar o espaço existente no buffer circular (espaço da memória que pode ser novamente escrito);
- Um para o controlador saber o número de dados que já pode ler da zona crítica.

Agora devem ser criados os processos filho (sensores). O programa deve criar 5 novos processos (número de sensores).

**Processo pai (Controller):**
O controller deve de para todas as leituras a serem feitas pelos sensores (num_sensores * num_leituras), realizar as seguintes tarefas:
- Esperar que existam dados para ler da zona crítica.
- Ganhar acesso exclusívo à zona crítica.
- Imprimir o valor lido pelo sensor e que está presente na posição atual a ler da zona crítica.
- Verificar se este sensor entrou ou saiu de modo alarme. Caso tenha acontecido, imprimir no ecrã a informar a situação e o número de sensores que estão em modo alarme.
- Limpar essa informação da memória partilhada.
- Incrementar o índice de leitura (passa ao índice de onde irá ler informação a seguir). (Não esquecer que se read_index == buffer_size, então read_index = 0)
- Libertar a zona crítica.
- Informar os sensores que já existe mais 1 espaço para colocarem informação na zona crítica.

Sinalizar a remoção das zonas criadas de memória partilhada.
Sinalizar a remoção dos semáforos criados.


**Processos filho (sensores):**
Deve existir uma estrutura para armazenar a informação necessária para cada sensor. (saber a leitura atual, a última leitura e se está no modo alarme)
Cada sensor deve realizar as seguintes tarefas (leituras) 6 vezes:
- Gerar um valor aleatório entre 0 e 100 que simula uma leitura do sensor.
- Esperar que exista espaço no buffer circular para escrever os dados necessários.
- Ganhar acesso exclusívo à zona crítica.
- Se o sensor ainda não estava previamente em modo alarme e a leitura atual for maior ou igual a 50, o sensor deve entrar em modo alarme.
- Se o sensor estava previamente em modo alarme e a leitura anterior e esta leitura atual forem menor do que 50, o alarme deve sair de modo alarme.
- Escrever em memória partilhada o valor lido, e se o sensor entrou ou saiu de modo alarme, esta informação deve também ser transmitida para o controller.
- Incrementar o índice de escrita na zona crítica que armazena os dados das leituras do sensores. (atenção ao buffer circular, se índice == buffer_size então índice = 0)
- Libertar a zona crítica incrementando o semáforo com essa função.
- Informar o processo controller que existe mais um valor que pode ser lido da zona crítica (incrementar semáforo).
- Dormir durante 1 segundo.

Por fim, todos os processos devem:
Fechar as zonas de memória partilhada utilizadas
Fechar todos os semáforos utilizados


**NOTA:** Nesta solução, não me parece que haja problema em não realizar o acesso exclusívo à zona crítica do processo pai, pois é o único processo que está a consumir dados da zona crítica. 