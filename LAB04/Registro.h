#ifndef REGISTRO_H
#define REGISTRO_H

#include <string>

class Registro {
public:
    std::string nome, sobrenome, telefone, dataNascimento;

    //tamanho fixo
    std::string packFixed() const;
    void unpackFixed(const std::string& buffer);

    //Registros de tamanho variavel com delimitadores
    std::string packDelimited() const;
    void unpackDelimited(const std::string& buffer);

    //Registros de tamanho variavel com descritor de tamanho
    std::string packSized() const;
    void unpackSized(const std::string& buffer);
};

#endif // REGISTRO_H
