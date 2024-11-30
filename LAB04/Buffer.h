#ifndef BUFFER_H
#define BUFFER_H

#include "Registro.h"
#include <fstream>
#include <stdexcept>

enum class Formato { FIXO, DELIMITADO, TAMANHO };

class Buffer {
private:
    Formato formato;

public:
    explicit Buffer(Formato formatoEscolhido) : formato(formatoEscolhido) {}

    void escreverRegistro(const Registro& reg, std::ofstream& out);
    Registro lerRegistro(std::ifstream& in);

    // Funções específicas de cada formato
    void escreverRegistroFixo(const Registro& reg, std::ofstream& out);
    Registro lerRegistroFixo(std::ifstream& in);

    void escreverRegistroDelimited(const Registro& reg, std::ofstream& out);
    Registro lerRegistroDelimited(std::ifstream& in);

    void escreverRegistroSized(const Registro& reg, std::ofstream& out);
    Registro lerRegistroSized(std::ifstream& in);
};

#endif // BUFFER_H
