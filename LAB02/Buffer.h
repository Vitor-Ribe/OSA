#ifndef BUFFER_H
#define BUFFER_H

#include "Registro.h"
#include <fstream>
#include <string>

class Buffer{
public:
    std::fstream arquivo;

    Buffer(const std::string& nomeArquivo, std::ios_base::openmode modo);

    ~Buffer();

    void escreverRegistro(Registro& reg);
    bool lerRegistro(Registro& reg);
};
#endif
