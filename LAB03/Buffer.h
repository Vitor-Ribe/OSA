#ifndef BUFFER_H
#define BUFFER_H

#include "Registro.h"
#include <fstream>
#include <stdexcept>
#include <string>

class Buffer {
private:
    std::fstream arquivo;

public:
    Buffer(const std::string& nomeArquivo, std::ios_base::openmode modo);
    ~Buffer();

    void escreverRegistro(const Registro& reg, bool comDescritor);
    bool lerRegistro(Registro& reg, bool comDescritor);
};

#endif
