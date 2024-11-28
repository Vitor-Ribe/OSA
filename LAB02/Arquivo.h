#ifndef ARQUIVO_H
#define ARQUIVO_H

#include "Registro.h"
#include "Buffer.h"
#include <vector>
#include <string>

class Arquivo{
public:
    std::vector<Registro> registros;
    void carregarCSV(const std::string& nomeArquivo);
    void salvarRegistrosBinario(const std::string& nomeArquivo);
};
#endif
