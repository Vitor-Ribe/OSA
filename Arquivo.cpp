#include "Arquivo.h"
#include <fstream>
#include <sstream>
#include <cstring>

void Arquivo::carregarCSV(const std::string& nomeArquivo){
    std::ifstream file(nomeArquivo); 
    std::string linha;

    while (std::getline(file, linha)){
        std::stringstream ss(linha);
        std::string nome;
        int idade;

        std::getline(ss, nome, ',');
        ss >> idade;

        Registro reg;
        std::strncpy(reg.nome, nome.c_str(), 49);
        reg.idade = idade;

        registros.push_back(reg);
    }
    file.close();
}

void Arquivo::salvarRegistrosBinario(const std::string& nomeArquivo){
    Buffer buffer(nomeArquivo, std::ios::out | std::ios::binary);
    for (auto& reg : registros){
        buffer.escreverRegistro(reg);
    }
}
