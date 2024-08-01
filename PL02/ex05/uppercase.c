/* Função para substituir todas as letras minúsculas por sua contraparte maiúscula. 
 * Recebe como parâmetro o endereço da string. 
*/
void uppercase(char *str) {	
	int i = 0;	

	// Ciclo para percorrer toda a String
	while (str[i] != '\0') {	
		// Verificar se a letra atual da String está compreendida entre o
		// valor ASCII do 'a' minúsculo e o 'z' minúsculo	
		if (str[i] >= 'a' && str[i] <= 'z') {
			// Colocação do valor ASCII para a letra maiúscula
			// correspondente à letra minúscula atual
			str[i] -= 32;
		}
		
		// Soma à variável i mais um
		i++;
	}
}