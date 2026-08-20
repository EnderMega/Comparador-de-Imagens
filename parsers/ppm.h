struct headerPPM {
	int largura, altura, maxVal;
	int offset;	// Se refere a quantos bytes do começo para o primeiro byte de dados
};

bool parseHeaderPPM(unsigned char* arquivo, headerPPM* h);

// retorno: offset
int prepararHeaderPPM(unsigned char* arquivo, headerPPM* h);
