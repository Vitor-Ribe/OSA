#ifndef REGISTRO_H
#define REGISTRO_H

class Registro {
public:
    char nome[50];
    int idade;

    void serializar(char* buffer);
    void desserializar(const char* buffer);
};
#endif 
