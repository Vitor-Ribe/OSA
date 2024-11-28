#include "Registro.h"
#include <cstring> 

void Registro::serializar(char* buffer) {
    memset(buffer, '\0', 54);
    strncpy(buffer, nome, 49);
    buffer[49] = '\0';
    memcpy(buffer + 50, &idade, sizeof(int));
}

void Registro::desserializar(const char* buffer) {
    strncpy(nome, buffer, 49);
    nome[49] = '\0';
    memcpy(&idade, buffer + 50, sizeof(int));
}
