#ifndef REGISTRO_H
#define REGISTRO_H

#include <string>
#include <sstream>

class Registro {
public:
    std::string nome;
    std::string sobrenome;
    std::string telefone;
    std::string dataNascimento;

    Registro() = default;

    // Serialização para formato delimitado
    std::string serializarDelimitadores() const;

    // Serialização para formato com descritor de tamanho
    std::string serializarDescritor() const;

    // Desserialização do formato delimitado
    void desserializarDelimitadores(const std::string& linha);

    // Desserialização do formato com descritor de tamanho
    void desserializarDescritor(const std::string& buffer);
};

#endif
