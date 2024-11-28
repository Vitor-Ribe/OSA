#include "Buffer.h"

Buffer::Buffer(const std::string& nomeArquivo, std::ios_base::openmode modo) {
    arquivo.open(nomeArquivo, modo);
    if (!arquivo.is_open()) {
        throw std::runtime_error("Não foi possível abrir o arquivo: " + nomeArquivo);
    }
}

Buffer::~Buffer() {
    if (arquivo.is_open()) {
        arquivo.close();
    }
}

void Buffer::escreverRegistro(const Registro& reg, bool comDescritor) {
    if (comDescritor) {
        std::string dados = reg.serializarDescritor();
        arquivo.write(dados.c_str(), dados.size());
    } else {
        std::string dados = reg.serializarDelimitadores();
        arquivo << dados;
    }
}

bool Buffer::lerRegistro(Registro& reg, bool comDescritor) {
    if (comDescritor) {
        size_t tamanho;
        if (!arquivo.read(reinterpret_cast<char*>(&tamanho), sizeof(size_t))) {
            return false;
        }
        std::string dados(tamanho, '\0');
        arquivo.read(&dados[0], tamanho);
        reg.desserializarDescritor(dados);
    } else {
        std::string linha;
        if (!std::getline(arquivo, linha)) {
            return false;
        }
        reg.desserializarDelimitadores(linha);
    }
    return true;
}
