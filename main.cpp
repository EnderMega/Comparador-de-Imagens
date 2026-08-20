#include <cstdlib>			// `malloc()`
#include <unistd.h>			// `write()`, `read()`, `exit()`, `close()`
#include <fcntl.h>			// `open()` e macros para ele: O_RDWR, O_CREAT, O_TRUNC, O_RDONLY.
#include <sys/stat.h>		// `fstat()`

#include <linux/limits.h>	// Macro PATH_MAX

#include "extras/extras.h"


//#define TESTE_LOG				// Imprime informações extras de debug

#define print(s) write(1, s, sizeof(s) - 1)
#define enumero(s) s > 47 && s < 58 

#define pixel(arquivo, header, extra) arquivo[header.offset + j * 3 + extra + header.largura * i * 3]
// Só tenho esses macros para um lugar específico, o teste de pixeis, que fica horroros se tiver que fazer pixel(...) ou literal,
//	tem um exemplo em cima (que foi utilizando expanção pelo editor de texto).
#define p1(ARG_X) pixel(arquivo1, header1, ARG_X)
#define p2(ARG_X) pixel(arquivo2, header2, ARG_X)
#define pf(ARG_X) pixel(arquivoFinal, menorHeader, ARG_X)



struct rgb {
	unsigned char r, g, b;
} cor_diferenca = { 255, 0, 0 };

rgb cor_semelhanca = { 0, 0, 0 };
rgb cor_tolerancia = { 0, 0, 0 };

int main()
{
	bool cor_usuario = false;
	bool semelhanca_usuario = false;
	bool nomeFinalUsuario = false;
	bool repetir = true;
	bool profundidadeSair = true;

	int fd = open("/proc/self/cmdline", O_RDONLY);
	if (fd != -1)
	{
		// Nome executável + 2 para `./`. x argumentos, x espaços, 3 cores que podems ser 3 dígitos para 'c' (cores), 1 para '\0'.
		char argumentos[2 + PATH_MAX + 4 + 4
			+ 3 * 3 + 3
			+ 1 + PATH_MAX + 1] = {};
		// WARNING: Talvez não esteja certo a exata quantidade de espaços entre os argumentos, mas isso é de menos.

		int tam = 0;
		int lidos;
		do {
			lidos = read(fd, argumentos, 100);
			tam += lidos;
		} while(lidos >= 100);
		close(fd);

		short nomeOffset = 0;
		for (; argumentos[nomeOffset] != 0; nomeOffset++)
			argumentos[nomeOffset] = 0;

		// . . .
		for (int i = nomeOffset; i < tam; i++)
		{
			if (argumentos[i] == 'h')	// Ajuda
			{
				print("Ajuda:\n\
Não é necessário traços para os argumentos.\
\n> \e[3mh\e[23m (Ajuda|Help) Mostra esta tela de ajuda.\
\n> \e[3mc=r,g,b\e[23m: (Cor) Define a cor que vai ser utilizada para mostrar o que tem de diferente no modo normal e o que tem de igual no modo de semelhança. \
Com \e[3m`r`\e[23m, \e[3m`g`\e[23m, \e[3m`b`\e[23m, sendo as cores \e[1;31mvermelho\e[0m, \e[1;32mverde\e[0m, \e[1;34mazul\e[0m, respectivamente, sem digitar os `. (Nota: Não tem que necessariamente seguir o esquema de '=' e ',', pode apenas espaçar os sub-argumetos, porém faz mais sentido assim)\
\n> \e[3ms\e[23m: (Semelhança) Em vez de comparar que as duas imagens possuem de diferente compara o que elas possuem de igual.\
\n> \e[3mn\e[23m: (Nome) Se o programa deve pedir o nome final/de saída para o arquivo.\
\n> \e[3mt=r,g,b\e[23m | \e[3mt=+~\e[23m: (Tolerancia) O quão diferente pode ser as cores antes de considera-las como diferente. \
Pode ser utilizado passando 3 valores, que são as diferença para os valores rgb individualmente ou pode ser passado `\e[3m+~\e[23m` para a soma das diferenças. (Nota: Não tem que necessariamente seguir o esquema de '=' e ',', pode apenas espaçar os sub-argumetos, porém faz mais sentido assim)\
\n> \e[3mr\e[23m: (\e[1m*Repetir\e[22m) \e[1mNão\e[22m repetir pedir nome ao não encontrar arquivo.. (Pode ser útil se se esse programa for chamado por outro já que o programa não vai digitar errado).\
\n> \e[3mp\e[23m: (\e[1m*Profundidade\e[22m) Não sair se a profundidade de bits dos arquivos são diferentes (Nota: Isso pode gerar erros ao analisar as imagens).\n");
				_exit(0);
			}
			else if (argumentos[i] == 'c') // Cor
			{
				for (int num = 0; num < 3; num++)
				{
					while (!(argumentos[i] >= '0' && argumentos[i] <= '9')) i++;
					*(((unsigned char*)&cor_diferenca) + (num * sizeof(unsigned char))) = matoi(argumentos + i);	// Isso é muito gambiarra, mas funciona.
					while (argumentos[i] >= '0' && argumentos[i] <= '9') i++;

					cor_semelhanca = cor_diferenca;
				}
			}
			else if (argumentos[i] == 's')	// Semelhança
				semelhanca_usuario = true;
			else if (argumentos[i] == 'n')	// Nome
				nomeFinalUsuario = true;
			else if (argumentos[i] == 't')	// Tolerancia
			{
				while (!(argumentos[i] >= '0' && argumentos[i] <= '9' || argumentos[i] == '+')) i++;

				for (int num = 0; num < 3; num++)
				{
					while (!(argumentos[i] >= '0' && argumentos[i] <= '9')) i++;
					*(((unsigned char*)&cor_tolerancia) + (num * sizeof(unsigned char))) = matoi(argumentos + i);	// Isso é muito gambiarra, mas funciona.
					while (argumentos[i] >= '0' && argumentos[i] <= '9') i++;
				}
			}
			else if (argumentos[i] == 'r')	// *Não* repetir
				repetir = false;
			else if (argumentos[i] == 'p')	// Não sair caso profundidade diferente
				profundidadeSair = false;
		}
	}
	else
		print("Erro ao ler argumentos, continuando sem eles.\n");

	char nome1[PATH_MAX];	// Não precisamos inicializar
	char nome2[PATH_MAX];
	char nomeFinal[PATH_MAX];
	
	
	int hArquivo1;
repetir1:
	print("Primeira imagem (incluir extensão .ppm): ");
	read(0, nome1, PATH_MAX - 1);
	for (int i = 0; i < PATH_MAX - 1; i++)
		if (nome1[i] == '\n')
		{
			nome1[i] = 0;
			break;
		}
	if ((hArquivo1 = open(nome1, O_RDONLY)) == -1)
	{
		print("Arquivo não encontrado.\n");
		if (repetir)
			goto repetir1;	// ""goto considered harmfull" considered harmfull"
		else
			_exit(1);
	}

	int hArquivo2;
repetir2:
	print("Segunda imagem  (incluir extensão .ppm): ");
	read(0, nome2, PATH_MAX - 1);
	for (int i = 0; i < PATH_MAX - 1; i++)
		if (nome2[i] == '\n')
		{
			nome2[i] = 0;
			break;
		}
	if ((hArquivo2 = open(nome2, O_RDONLY)) == -1)
	{
		print("Arquivo não encontrado.\n");
		if (repetir)
			goto repetir2;	// ""goto considered harmfull" considered harmfull"
		else
			_exit(2);
	}
	
	struct stat tam1;
	struct stat tam2;
	fstat(hArquivo1, &tam1);
	fstat(hArquivo2, &tam2);

	unsigned char* arquivo1 = (unsigned char*)malloc(tam1.st_size);
	unsigned char* arquivo2 = (unsigned char*)malloc(tam2.st_size);

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

	headerPPM header1, header2, menorHeader;	// `menorHeader` também é o header final
	{
		bool primeiro = parseHeaderPPM(arquivo1, &header1);
		bool segundo = parseHeaderPPM(arquivo2, &header2);

		if (!primeiro && !segundo)
		{
			print("Nem um dos dois arquivos possui o número mágico (P6) ou está corrompido.\n");
			_exit(3);
		}
		else if (!primeiro)
		{
			print( "O primeiro arquivo não possui o número mágico (P6) ou está corrompido.\n");
			_exit(4);
		}
		else if (!segundo)
		{
			print( "O segundo arquivo não possui o número mágico (P6) ou está corrompido.\n");
			_exit(5);
		}
	}

#ifdef TESTE_LOG
	{
		char tempLog[255] = {};

		print("Largura 1: ");
		print(mitoa(header1.largura, tempLog, 10));
		for (int i = 0; i < 255; i++)
			tempLog[i] = 0;
		print("\nAltura 1: ");
		print(mitoa(header1.altura, tempLog, 10));
		for (int i = 0; i < 255; i++)
			tempLog[i] = 0;
		print("\nProfundidade 1: ");
		print(mitoa(header1.maxVal, tempLog, 10));
		for (int i = 0; i < 255; i++)
			tempLog[i] = 0;
		print("\nOffset 1: ");
		print(mitoa(header1.offset, tempLog, 10));
		for (int i = 0; i < 255; i++)
			tempLog[i] = 0;

		/*********************************************/

		print("\nLargura 2: ");
		print(mitoa(header2.largura, tempLog, 10));
		for (int i = 0; i < 255; i++)
			tempLog[i] = 0;
		print("\nAltura 2: ");
		print(mitoa(header2.altura, tempLog, 10));
		for (int i = 0; i < 255; i++)
			tempLog[i] = 0;
		print("\nProfundidade 2: ");
		print(mitoa(header2.maxVal, tempLog, 10));
		for (int i = 0; i < 255; i++)
			tempLog[i] = 0;
		print("\nOffset 2: ");
		print(mitoa(header2.offset, tempLog, 10));
		for (int i = 0; i < 255; i++)
			tempLog[i] = 0;
		print("\n");
	}
#endif
	
	if (header1.maxVal != header2.maxVal)
	{
		if (profundidadeSair)
		{
			print("\n\e[1mERRO!!!\e[22m\nO valor de profundidade dos arquivos é diferente.\n");
			_exit(6);
		}
		else
			print("\n\e[1mAVISO!!!\e[22m\nO valor de profundidade dos arquivos é diferente.\n");
	}

	if (header1.largura != header2.largura || header1.altura != header2.altura)
	{
		print("\e[1mAs imagens possuem tamanhos diferentes!\e[22m Será analisado a intersecção delas.\n");

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

	unsigned char* arquivoFinal;
	{
		//   Ao mesmo tempo que é meio gambiarra é genial. Vamos preparar um falso header PPM e isso já calcula o offset pra gente (se não fizesse desse jeito ia ter que
		// repetir tudo que tem no `prepararPPM` de qualquer jeito).
		unsigned char str[3 + 50 + 1 + 50 + 1 + 3 + 1];	// 3 do número mágico, 50 para a largura e altura, com 1 espaço no meio e '\n' depois e 3 para a profundidade (1 a 255) com 1 nova linha depois.
		// Número mágico é 3 linhas, incluindo o '\n', os +1 são divisórias obrigatórias, com elas podendo ser ' ' ou '\n', 50 de máximo de número de caracteres é por que sim ...
		// eu coloquei como máximo de 50 caracteres no meu `parseHeader` e a profundidade vai de 1 caractere a 3 (1-255), não pode menos ou mais.
		// Teóricamente pode passar de 255, mas mesmo assim não vai chegar a 4 dígitos. O único problema seria que acima de 255 cada parte da tupla é 2 bytes, algo que não verificamos.

		arquivoFinal = (unsigned char*)malloc(menorHeader.largura * menorHeader.altura * 3 + prepararHeaderPPM(str, &menorHeader));
	}

	menorHeader.offset = prepararHeaderPPM(arquivoFinal, &menorHeader);	// ... é meio gambiarra sim.

	bool diferentes = false;

	for (int i = 0; i < menorHeader.altura; i++)
	{
		for (int j = 0; j < menorHeader.largura; j++)
		{
			// inicial <= final - t && final + t <= inicial [Nesse caso final é o pixel 2]
			// if ((arquivo1[header1.offset + j * 3 + 0 + header1.largura * i * 3] <= arquivo2[header2.offset + j * 3 + 0 + header2.largura * i * 3] - cor_tolerancia.r &&
			// 	arquivo2[header2.offset + j * 3 + 0 + header2.largura * i * 3] + cor_tolerancia.r <= arquivo1[header1.offset + j * 3 + 0 + header1.largura * i * 3]) &&
			// 	// - - -
			// 	(arquivo1[header1.offset + j * 3 + 1 + header1.largura * i * 3] <= arquivo2[header2.offset + j * 3 + 1 + header2.largura * i * 3] - cor_tolerancia.g &&
			// 	arquivo2[header2.offset + j * 3 + 1 + header2.largura * i * 3] + cor_tolerancia.g <= arquivo1[header1.offset + j * 3 + 1 + header1.largura * i * 3]) &&
			// 	// - - -
			// 	(arquivo1[header1.offset + j * 3 + 2 + header1.largura * i * 3] <= arquivo2[header2.offset + j * 3 + 2 + header2.largura * i * 3] - cor_tolerancia.b &&
			// 	arquivo2[header2.offset + j * 3 + 2 + header2.largura * i * 3] + cor_tolerancia.b <= arquivo1[header1.offset + j * 3 + 2 + header1.largura * i * 3]))
			
			// if ((p1(0) <= p2(0) - cor_tolerancia.r && p2(0) + cor_tolerancia.r <= p1(0)) &&
			// 	(p1(1) <= p2(1) - cor_tolerancia.g && p2(1) + cor_tolerancia.g <= p1(1)) &&
			// 	(p1(2) <= p2(2) - cor_tolerancia.b && p2(2) + cor_tolerancia.b <= p1(2)) )
			
			if (pixel(arquivo1, header1, 0) == pixel(arquivo2, header2, 0) &&
				pixel(arquivo1, header1, 1) == pixel(arquivo2, header2, 1) &&
				pixel(arquivo1, header1, 2) == pixel(arquivo2, header2, 2))
			{
				if (semelhanca_usuario)
				{
					pixel(arquivoFinal, menorHeader, 0) = 255;
					pixel(arquivoFinal, menorHeader, 1) = 255;
					pixel(arquivoFinal, menorHeader, 2) = 255;
				}
				else
				{
					pixel(arquivoFinal, menorHeader, 0) = pixel(arquivo1, header1, 0);
					pixel(arquivoFinal, menorHeader, 1) = pixel(arquivo1, header1, 1);
					pixel(arquivoFinal, menorHeader, 2) = pixel(arquivo1, header1, 2);
				}
			}
			else
			{
				if (semelhanca_usuario)
				{
					pixel(arquivoFinal, menorHeader, 0) = cor_semelhanca.r;
					pixel(arquivoFinal, menorHeader, 1) = cor_semelhanca.g;
					pixel(arquivoFinal, menorHeader, 2) = cor_semelhanca.b;
				}
				else
				{
					pixel(arquivoFinal, menorHeader, 0) = cor_diferenca.r;
					pixel(arquivoFinal, menorHeader, 1) = cor_diferenca.g;
					pixel(arquivoFinal, menorHeader, 2) = cor_diferenca.b;
				}

				/*
				cor_diferenca = corFunc();
				pixel(arquivoFinal, menorHeader, 0) = cor_erro.r;
				pixel(arquivoFinal, menorHeader, 1) = cor_erro.g;
				pixel(arquivoFinal, menorHeader, 2) = cor_erro.b;
				*/

				diferentes = true;
			}
		}
	}

	if (diferentes)
		print("Os arquivos são diferentes.\n");

	if (nomeFinalUsuario)
	{
		print("Digite o nome para o arquivo final (Ctrl+c caso não deseje salvar o arquivo): ");
		read(0, nomeFinal, PATH_MAX - 1);
		for (int i = 0; i < PATH_MAX - 1; i++)
			if (nomeFinal[i] == '\n')
			{
				nomeFinal[i] = 0;
				break;
			}
	}
	else
		copiar("final.ppm", nomeFinal);
	
	int hArquivo3 = open(nomeFinal, O_CREAT | O_TRUNC | O_RDWR, 0644);
	write(hArquivo3, arquivoFinal, menorHeader.offset + menorHeader.largura * menorHeader.altura * 3);

	_exit(0);
}
