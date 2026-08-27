struct headerPPM {
	int largura, altura, maxVal;
	int offset;	// Se refere a quantos bytes do começo para o primeiro byte de dados
};

// Extrai o header do arquivo
bool parseHeaderPPM(unsigned char* arquivo, headerPPM* h);

// Coloca o header em um arquivo e calcula o offset
// `sep` é o separador entre largura e altura, padrão espaço `' '`
// retorno: offset
int prepararHeaderPPM(unsigned char* arquivo, headerPPM h, char sep = ' ');

// Calcula o offset com apenas as informações do header (isso quer dizer que não considera comentários no ppm)
int calcOffset(headerPPM* h);

// TODO: Fazer para poder inserir texto e qualquer posição que é possível inserir texto
// void inserirTexto(...)
