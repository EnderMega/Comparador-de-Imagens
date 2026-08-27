#include <cstdlib>			// `malloc()`
#include <unistd.h>			// `write()`, `read()`, `exit()`, `close()`
#include <fcntl.h>			// `open()` e macros para ele: O_RDWR, O_CREAT, O_TRUNC, O_RDONLY.
#include <sys/stat.h>		// `fstat()`

#include <linux/limits.h>	// Macro PATH_MAX

#include "../utils/matematica.h"
#include "../parsers/ppm.h"

#define print(s) write(1, s, sizeof(s) - 1)



struct corStruct {
	unsigned char r, g, b;
} cor;

int main()
{
	char ladosStr[2][21];	// Pelo que e contei, 2^64 (long long) pode ter no máximo 20 casas decimais e +1 para o '\0'.
	print("Largura da imagem: ");
	read(0, ladosStr[0], 20);
	print("Altura da imagem: ");
	read(0, ladosStr[1], 20);
	
	int ladosInt[2] = { matoi((const char*)ladosStr[0]), matoi((const char*)ladosStr[0]) };

	constexpr int profundidade = 255;
	constexpr int tamanhoProf = 3;

	headerPPM header = { ladosInt[0], ladosInt[1], profundidade };
	calcOffset(&header);

	char corStr[12];
	print("Digite a cor da imagem: ");
	read(0, corStr, 11);
	// literalmente cópia do diffimagem
	{
		int i = 0;
		for (int num = 0; num < 3; num++)
		{
			while (!(corStr[i] >= '0' && corStr[i] <= '9')) i++;
			*(((unsigned char*)&cor) + (num * sizeof(unsigned char))) = matoi(corStr + i);	// Isso é muito gambiarra, mas funciona.
			while (corStr[i] >= '0' && corStr[i] <= '9') i++;
		}
	}

	unsigned char* imagem = (unsigned char*)malloc(header.offset + header.largura * header.altura);
	prepararHeaderPPM(imagem, header);
	for (int i = 0; i < header.largura * header.altura; i++)
	{
		imagem[header.offset + i + 0] = cor.r;
		imagem[header.offset + i + 1] = cor.g;
		imagem[header.offset + i + 2] = cor.b;
	}
	
	int fd = open("Saida.ppm", O_CREAT | O_TRUNC | O_RDWR, 0644);
	write(fd , imagem, header.offset + header.largura * header.altura);

	_exit(0);
}
