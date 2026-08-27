// Essa pega o tamanho de `origem` até o '\0' incluindo o '\0'
// Retorna o número de bytes copiados
int copiar(const char* origem, char* destino)
{
	int tam = 0;
	for (; origem[tam] != 0; tam++);
	++tam;

	for (int i = 0; i < tam; i++)
	{
		destino[i] = origem[i];
	}

	return tam;
}

// Essa recebe o tamanho de `origem`, para caso não quera parar no \0
void copiar(const char* origem, char* destino, int tam)
{
	for (int i = 0; i < tam; i++)
	{
		destino[i] = origem[i];
	}
}
