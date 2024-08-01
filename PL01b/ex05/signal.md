| Signal    | Número | Ação                                                                                                                   |
|-----------|--------|------------------------------------------------------------------------------------------------------------------------|
| SIGHUP    | 1      | Desconecta um processo do processo pai.                                                                                 |
| SIGINT    | 2      | Terminar o processo; Interrupção gerada ao pressionar CTRL-C.                                                           |
| SIGQUIT   | 3      | Como o SIGINT, com core dump (despejo da memória principal).                                                            |
| SIGILL    | 4      | Criar imagem principal; Instrução ilegal.                                                                               |
| SIGTRAP   | 5      | Termina processos quando um debugger está em execução.                                                                  |
| SIGABRT   | 6      | Aborta um processo.                                                                                                     |
| SIGFPE    | 8      | Termina processos que tentam executar operações erradas (por ex dividir um numero por 0).                               |
| SIGKILL   | 9      | Terminar o processo; Mata programa.                                                                                     |
| SIGUSR1   | 10     | Terminar o processo; Sinal definido pelo usuário 1.                                                                     |
| SIGSEGV   | 11     | Criar imagem principal; Violação de segmentação (acesso ilegal à memória).                                              |
| SIGUSR2   | 12     | Terminar o processo; Sinal definido pelo usuário 1.                                                                     |
| SIGPIPE   | 13     | Termina um processo que tenta escrever num pipe sem um processo ligado à outra extremidade.                             |
| SIGALRM   | 14     | Terminar o processo; Temporizador em tempo real expirado.                                                               |
| SIGTERM   | 15     | Sinal enviado a um processo para solicitar o seu fim, ele pode ser capturado e interpretado ou ignorado pelo processo.  |
| SIGCHLD   | 17     | Notifica um processo quando um processo filha termina.                                                                  |
| SIGCONT   | 18     | O processo continua a sua execução depois de ter sido parado.                                                           |
| SIGSTOP   | 19     | Interrompe um processo.                                                                                                 |
| SIGTSTP   | 20     | Termina a execução de um processo para continuar depois.                                                                |
| SIGTTIN   | 21     | Notifica um processo quando este tenta ler do tty enquanto está em segundo plano.                                       |
| SIGTTOU   | 22     | Notifica um processo quando este tenta escrever a partir do tty, em segundo plano.                                      |