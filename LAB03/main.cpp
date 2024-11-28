#include "Registro.h"
#include "Buffer.h"
#include <iostream>
#include <fstream>
#include <vector>

void carregarTXT(const std::string& nomeArquivo, std::vector<Registro>& registros) {
    std::ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        throw std::runtime_error("Não foi possível abrir o arquivo de entrada: " + nomeArquivo);
    }

    std::string linha;
    std::getline(arquivo, linha); // Ignorar cabeçalho
    while (std::getline(arquivo, linha)) {
        Registro reg;
        reg.desserializarDelimitadores(linha);
        registros.push_back(reg);
    }
}

void salvarRegistrosBinario(const std::string& nomeArquivo, const std::vector<Registro>& registros) {
    Buffer buffer(nomeArquivo, std::ios::out | std::ios::binary);
    for (const auto& reg : registros) {
        buffer.escreverRegistro(reg, true); // Com descritor
    }
}

void salvarRegistrosDelimitadores(const std::string& nomeArquivo, const std::vector<Registro>& registros) {
    std::ofstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        throw std::runtime_error("Não foi possível abrir o arquivo de saída: " + nomeArquivo);
    }

    arquivo << "Nome|Sobrenome|Telefone|Nascimento\n"; // Cabeçalho
    for (const auto& reg : registros) {
        arquivo << reg.serializarDelimitadores();
    }
}

int main() {
    try {
        std::vector<Registro> registros;

        // Carregar dados do arquivo TXT
        carregarTXT("Dados.txt", registros);

        // Salvar registros em formato binário
        salvarRegistrosBinario("saida.bin", registros);

        // Ler e exibir registros do binário
        Buffer buffer("saida.bin", std::ios::in | std::ios::binary);
        Registro reg;

        std::cout << "Registros lidos do arquivo binário:" << std::endl;
        while (buffer.lerRegistro(reg, true)) {
            std::cout << "Nome: " << reg.nome << ", Sobrenome: " << reg.sobrenome
                      << ", Telefone: " << reg.telefone
                      << ", Data de Nascimento: " << reg.dataNascimento << std::endl;
        }

        // Salvar registros em formato delimitado
        salvarRegistrosDelimitadores("saida_delimitadores.txt", registros);

    } catch (const std::exception& ex) {
        std::cerr << "Erro: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
