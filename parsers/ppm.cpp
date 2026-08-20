#include "../extras/extras.h"

struct headerPPM {
	int largura, altura, maxVal;
	int offset;	// Se refere a quantos bytes do começo para o primeiro byte de dados
};

/*
- Formato do cabeçalho:
P6\n
`largura` `altura`\n
`profundidade de bits`\n
ff ff ff ...
ff ff ff ...
...

Cada linha possui altura tuplas(triplas).

*Pode conter comentários, em literalmente qualquer lugar antes da profundidade de cores, mas eu creio que depois do número mágico.

P6\n
# comentário
123 # comentário
# comentário
123 # comentário
# comentário
255
*/
unsigned char parseHeaderPPM(unsigned char* arquivo, headerPPM* h)
{
	char num[51] = {};	// Se o número tiver mais de 50 dígitos eu choro. Eu até coloquei o +1 pra garantir o '\0' :(

	if (!(arquivo[0] == 'P' && arquivo[1] == '6' && (arquivo[2] == '\n' || arquivo[2] == ' ')))
		return false;

	int i = 3;
	int numtokens = 0;
	while (numtokens < 3)	// Largura, altura e profundidade
	{
		// Infelizmente os testes tem que ser separados, pois pode ter espaços dentro de um comentário que como consequência causam colisões entre os testes
		if (arquivo[i] == '#')
		{
			do { i++; } while (arquivo[i] != '\n');
			++i;
		}
		if (arquivo[i] == ' ')	// Sei lá se eu deveria me importar com mais de um espaço, mas eu vou
		{
			do { i++; } while (arquivo[i] != ' ' || arquivo[i] != '\n');
			++i;
		}

		// Se é um número
		if (arquivo[i] > 47 && arquivo[i] < 58)
		{
			int j;
			for (j = 0; arquivo[j + i] != ' ' && arquivo[j + i] != '\n'; j++)
				num[j] = arquivo[j + i];

			i += j + 1;	// +1 pra já pular e não precisar testar pelo ' ' que fez sair do loop

			// Largura
			if (numtokens == 0)
				h->largura = matoi(num);
			// Altura
			else if (numtokens == 1)
				h->altura = matoi(num);
			// Profundidade
			else
				h->maxVal = matoi(num);

			for (int aaa = 0; num[aaa] != 0; aaa++)
				num[aaa] = 0;

			++numtokens;
		}
		else
		{
			return false;
		}
	}

	h->offset = i;

	return true;
}

int prepararHeaderPPM(unsigned char* arquivo, headerPPM* h)
{
	// Número mágico
	arquivo[0] = 'P';
	arquivo[1] = '6';
	arquivo[2] = '\n';
	//      ^-+1-v
	int offset = 3;

	offset += mitoa2(h->largura, (char*)(arquivo + offset), 10);
	arquivo[offset++] = ' ';

	offset += mitoa2(h->altura, (char*)(arquivo + offset), 10);
	arquivo[offset++] = '\n';

	offset += mitoa2(h->maxVal, (char*)(arquivo + offset), 10);
	arquivo[offset++] = '\n';

	return offset;
}
