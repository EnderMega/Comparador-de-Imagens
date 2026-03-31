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

char* itoa(int num, char* str, int base)
{
	int i = 0;
    bool isNegative = false;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    if (num < 0 && base == 10) {
        isNegative = true;
        num = -num;
    }

    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    if (isNegative)
        str[i++] = '-';

    str[i] = '\0';

    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        end--;
        start++;
    }

    return str;
}

// String terminada em '\0' ou possui um espaço (' ') no final.
int myAtoi(char* str)
{
    int res = 0;

    for (int i = 0; str[i] != 0 || str[i] != ' '; ++i)
        res = res * 10 + str[i] - '0';

    return res;
}
