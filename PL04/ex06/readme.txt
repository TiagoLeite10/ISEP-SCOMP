1. Control de acesso a recursos com capacidade limitada, pois existe o limite máximo de diferença de 2 prints entre dois processos.

2. Para realizar este exercício começamos por criar 2 semáforos, em que cada um deles irá controlar se o processo pai/filho pode 
escrever a sua letra naquele momento. Estes dois semáforos são inicializados com o valor 2.
Após a criação dos dois semáforos, o processo principal cria um processo filho. O processo pai e filho entram num ciclo infinito.
Cada um deles vai ter de realizar a subtração do seu próprio semáforo (sem_wait()), em que após realizar esta subtração, imprimem
no ecrã a sua letra ('S' no processo pai e 'C' no processo filho).
Após imprimir no ecrã, o processo deve incrementar o valor do semáforo que controla a escrita do outro processo. 
Desta forma garantimos que as letras escritas no ecrã, no máximo vão diferir por 2.
