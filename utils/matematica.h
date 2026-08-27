// Essa pega o tamanho de `origem` até o '\0' incluindo o '\0'
// Retorna o número de bytes copiados
int copiar(const char* origem, char* destino);

// Essa recebe o tamanho de `origem`, para caso não quera parar no \0
void copiar(const char* origem, char* destino, int tam);

int matoi(const char* str);

// Retorna a string
char* mitoa(int num, char* str, int base);

// Retorna o tamanho
int mitoa2(int num, char* str, int base);
