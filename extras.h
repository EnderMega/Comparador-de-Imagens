// Essa pega o tamanho de `origem` até o '\0' incluindo o '\0'
// Retorna o número de bytes copiados
#include <sys/types.h>
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

int matoi(const char* str) {
    int result = 0;
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    return result;
}

// Retorna a string
char* mitoa(int num, char* str, int base)
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

// Retorna o tamanho
int mitoa2(int num, char* str, int base)
{
	int i = 0;
    bool isNegative = false;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return i;
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

    return i;
}

struct rgb {
	char r, g, b;
};

rgb corFunc()
{
	static short r = 0;
	static short g = 0;
	static short b = 0;

	r += 51;
	if (r > 0xff)
	{
		r = 0;
		g += 51;
	}
	if (g > 0xff)
	{
		g = 0;
		b += 51;
	}
	if (b > 0xff)
	{
		b = 0;
	}

	rgb ret;
	ret = { (char)r, (char)g, (char)b };

	return ret;
}
