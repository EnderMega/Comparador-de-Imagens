#include <cstdlib>		// `malloc()`
#include <unistd.h>		// `write()`, `read()`, `exit()`, `close()`
#include <fcntl.h>		// `open()` e macros para ele: O_RDWR, O_CREAT, O_TRUNC, O_RDONLY.
#include <sys/stat.h>	// `fstat()`

#include <linux/limits.h>	// Macro PATH_MAX

#include "extras/extras.h"


//#define NOMEFINAL				// Usar `final.ppm` para nome final em vez de pedir para o usuário.
//#define TESTE					// Nomes padrão para as entradas
//#define TESTE_LOG				// Imprime informações extras de debug
#define SAIR_DIFF_PROFUNDIDADE	// Se deve terminar a execução caso os arquivo possuiam profundidades diferentes

#define print(s) write(1, s, sizeof(s) - 1)
#define enumero(s) s > 47 && s < 58 

#define pixel(arquivo, header, extra) arquivo[header.offset + j * 3 + extra + header.largura * i * 3]



struct header {
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
unsigned char parseHeader(char* arquivo, header* h)
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

int prepararPPM(char* arquivo, header* h)
{
	// Número mágico
	arquivo[0] = 'P';
	arquivo[1] = '6';
	arquivo[2] = '\n';
	//      ^-+1-v
	int offset = 3;

	offset += mitoa2(h->largura, arquivo + offset, 10);
	arquivo[offset++] = ' ';

	offset += mitoa2(h->altura, arquivo + offset, 10);
	arquivo[offset++] = '\n';

	offset += mitoa2(h->maxVal, arquivo + offset, 10);
	arquivo[offset++] = '\n';

	return offset;
}

struct rgb {
	unsigned char r, g, b;
} cor_erro;

int main()
{
	// TODO: Argumentos
	int fd = open("/proc/self/cmdline", O_RDONLY);
	if (fd != -1)
	{
		// Tamanho do arquivo que é sempre o primeiro argumento, +2 para caso `./` a partir do console.
		// char temp[2 + PATH_MAX + tam_argumento1 + ... + quant + 1] = {};
		char temp[PATH_MAX * 5] = {};	// XXX: Temp

		read(fd, temp, 500);
		close(fd);

		short argsOffset = 0;
		for (; temp[argsOffset] != 0; argsOffset++)
			temp[argsOffset] = 0;

		// . . .
	}
	else
		print("Erro ao ler argumentos, continuando sem eles.\n");

	char nome1[PATH_MAX];	// Não precisamos inicializar
	char nome2[PATH_MAX];
	char nomeFinal[PATH_MAX];
#ifdef TESTE
	copiar("fotos/perlica1.ppm", nome1);
#else
	write(1, "Primeira imagem (incluir extensão .ppm): ", 42);
	read(0, nome1, PATH_MAX - 1);
	for (int i = 0; i < PATH_MAX - 1; i++)
		if (nome1[i] == '\n')
		{
			nome1[i] = 0;
			break;
		}
#endif
	int hArquivo1;
	if ((hArquivo1 = open(nome1, O_RDONLY)) == -1)
	{
		write(1, "Arquivo não encontrado.\n", 25);

		_exit(1);
	}

#ifdef TESTE
	copiar("fotos/perlica2.ppm", nome2);
#else
	write(1, "Segunda imagem  (incluir extensão .ppm): ", 42);
	read(0, nome2, PATH_MAX - 1);
	for (int i = 0; i < PATH_MAX - 1; i++)
		if (nome2[i] == '\n')
		{
			nome2[i] = 0;
			break;
		}
#endif
	int hArquivo2;
	if ((hArquivo2 = open(nome2, O_RDONLY)) == -1)
	{
		write(1, "Arquivo não encontrado.\n", 25);

		_exit(2);
	}

#ifdef NOMEFINAL
	write(1, "Nome do arquivo final (incluir extensão .ppm): ", 48);
	read(0, nomeFinal, 255);
	for (int i = 0; i < 255; i++)
		if (nomeFinal[i] == '\n')
		{
			nomeFinal[i] = 0;
			break;
		}
#else
	copiar("final.ppm", nomeFinal);
#endif
	
	struct stat tam1;
	struct stat tam2;
	fstat(hArquivo1, &tam1);
	fstat(hArquivo2, &tam2);

	char* arquivo1 = (char*)malloc(tam1.st_size);
	char* arquivo2 = (char*)malloc(tam2.st_size);

	read(hArquivo1, arquivo1, tam1.st_size);
	read(hArquivo2, arquivo2, tam2.st_size);

	// Vamos nos basear na menor intersecção das duas imagens
	/*
		 _______________
		|       |       |
		|   x   |       |
		|_______|_______|
		|       |
		|       |
		|_______|		Vamos apenas nos importar com a parte com o 'x'.
		Basicamente, a menor largura e menor altura.
	*/

	header header1, header2, menorHeader;	// `menorHeader` também é o header final
	{
		bool primeiro = parseHeader(arquivo1, &header1);
		bool segundo = parseHeader(arquivo2, &header2);

		if (!primeiro && !segundo)
		{
			write(1, "Nem um dos dois arquivos possui o número mágico (P6) ou está corrompido.\n", 76);
			_exit(3);
		}
		else if (!primeiro)
		{
			write(1, "O primeiro arquivo não possui o número mágico (P6) ou está corrompido.\n", 75);
			_exit(4);
		}
		else if (!segundo)
		{
			write(1, "O segundo arquivo não possui o número mágico (P6) ou está corrompido.\n", 74);
			_exit(5);
		}
	}

#ifdef TESTE_LOG
	{
		char temp[255] = {};

		write(1, "Largura 1: ", 11);
		write(1, mitoa(header1.largura, temp, 10), 255);
		for (int i = 0; i < 255; i++)
			temp[i] = 0;
		write(1, "\nAltura 1: ", 11);
		write(1, mitoa(header1.altura, temp, 10), 255);
		for (int i = 0; i < 255; i++)
			temp[i] = 0;
		write(1, "\nProfundidade 1: ", 17);
		write(1, mitoa(header1.maxVal, temp, 10), 255);
		for (int i = 0; i < 255; i++)
			temp[i] = 0;
		write(1, "\nOffset 1: ", 11);
		write(1, mitoa(header1.offset, temp, 10), 255);
		for (int i = 0; i < 255; i++)
			temp[i] = 0;

		/*********************************************/

		write(1, "\nLargura 2: ", 12);
		write(1, mitoa(header2.largura, temp, 10), 255);
		for (int i = 0; i < 255; i++)
			temp[i] = 0;
		write(1, "\nAltura 2: ", 11);
		write(1, mitoa(header2.altura, temp, 10), 255);
		for (int i = 0; i < 255; i++)
			temp[i] = 0;
		write(1, "\nProfundidade 2: ", 17);
		write(1, mitoa(header2.maxVal, temp, 10), 255);
		for (int i = 0; i < 255; i++)
			temp[i] = 0;
		write(1, "\nOffset 2: ", 11);
		write(1, mitoa(header2.offset, temp, 10), 255);
		for (int i = 0; i < 255; i++)
			temp[i] = 0;
		write(1, "\n", 1);
	}
#endif
	
	if (header1.maxVal != header2.maxVal)
	{
#ifndef SAIR_DIFF_PROFUNDIDADE
		write(1, "\n\e[1mAVISO!!!\e[22m\nO valor de profundidade dos arquivos é diferente.\n\e[3m[SAIR_DIFF_PROFUNDIDADE \e[1mNÃO\e[22m definido]\e[23m\n", 127);
#else
		write(1, "\n\e[1mERRO!!!\e[22m\nO valor de profundidade dos arquivos é diferente.\n\e[3m[SAIR_DIFF_PROFUNDIDADE definido]\e[23m\n\n", 113);
		_exit(6);
#endif
	}

	if (header1.largura != header2.largura || header1.altura != header2.altura)
	{
		write(1, "\e[1mAs imagens possuem tamanhos diferentes!\e[22m Será analisado a intersecção delas.\n", 88);

		menorHeader.largura = header1.largura < header2.largura ? header1.largura : header2.largura;
		menorHeader.altura = header1.altura < header2.altura ? header1.altura : header2.altura;
	}
	else
	{
		menorHeader.largura = header1.largura;
		menorHeader.altura = header1.altura;
	}
	menorHeader.maxVal = header1.maxVal;	// Para o arquivo final nós vamos sempre copiar do primeiro arquivo menos quando é diferente.
											// Em geral não faz diferença de qual a gente copia; como o primeiro arquivo é a referência vamos copiar o seu maxVal.

	char* arquivoFinal;
	{
		//   Ao mesmo tempo que é meio gambiarra é genial. Vamos preparar um falso header PPM e isso já calcula o offset pra gente (se não fizesse desse jeito ia ter que
		// repetir tudo que tem no `prepararPPM` de qualquer jeito).
		char str[3 + 50 + 1 + 50 + 1 + 3 + 1];	// 3 do número mágico, 50 para a largura e altura, com 1 espaço no meio e '\n' depois e 3 para a profundidade (1 a 255) com 1 nova linha depois.
		// Número mágico é 3 linhas, incluindo o '\n', os +1 são divisórias obrigatórias, com elas podendo ser ' ' ou '\n', 50 de máximo de número de caracteres é por que sim ...
		// eu coloquei como máximo de 50 caracteres no meu `parseHeader` e a profundidade vai de 1 caractere a 3 (1-255), não pode menos ou mais.
		// Teóricamente pode passar de 255, mas mesmo assim não vai chegar a 4 dígitos. O único problema seria que acima de 255 cada parte da tupla é 2 bytes, algo que não verificamos.

		arquivoFinal = (char*)malloc(menorHeader.largura * menorHeader.altura * 3 + prepararPPM(str, &menorHeader));
	}

	menorHeader.offset = prepararPPM(arquivoFinal, &menorHeader);	// ... é meio gambiarra sim.

	bool diferentes = false;

	for (int i = 0; i < menorHeader.altura; i++)
	{
		for (int j = 0; j < menorHeader.largura; j++)
		{
			if (pixel(arquivo1, header1, 0) == pixel(arquivo2, header2, 0) &&
				pixel(arquivo1, header1, 1) == pixel(arquivo2, header2, 1) &&
				pixel(arquivo1, header1, 2) == pixel(arquivo2, header2, 2))
			{
				pixel(arquivoFinal, menorHeader, 0) = pixel(arquivo1, header1, 0);
				pixel(arquivoFinal, menorHeader, 1) = pixel(arquivo1, header1, 1);
				pixel(arquivoFinal, menorHeader, 2) = pixel(arquivo1, header1, 2);
			}
			else
			{
				pixel(arquivoFinal, menorHeader, 0) = cor_erro.r;
				pixel(arquivoFinal, menorHeader, 1) = cor_erro.g;
				pixel(arquivoFinal, menorHeader, 2) = cor_erro.b;

				/*
				cores = corFunc();
				pixel(arquivoFinal, menorHeader, 0) = cores.r;
				pixel(arquivoFinal, menorHeader, 1) = cores.g;
				pixel(arquivoFinal, menorHeader, 2) = cores.b;
				*/

				diferentes = true;
			}
		}
	}

	if (diferentes)
		write(1, "Os arquivos são diferentes.\n", 29);

	int hArquivo3 = open(nomeFinal, O_CREAT | O_TRUNC | O_RDWR, 0644);
	write(hArquivo3, arquivoFinal, menorHeader.offset + menorHeader.largura * menorHeader.altura * 3);

	_exit(0);
}
