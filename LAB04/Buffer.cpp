#include "Buffer.h"

// Escrever registro no formato selecionado
void Buffer::escreverRegistro(const Registro& reg, std::ofstream& out) {
    switch (formato) {
        case Formato::FIXO:
            escreverRegistroFixo(reg, out);
            break;
        case Formato::DELIMITADO:
            escreverRegistroDelimited(reg, out);
            break;
        case Formato::TAMANHO:
            escreverRegistroSized(reg, out);
            break;
    }
}

// Ler registro no formato selecionado
Registro Buffer::lerRegistro(std::ifstream& in) {
    switch (formato) {
        case Formato::FIXO:
            return lerRegistroFixo(in);
        case Formato::DELIMITADO:
            return lerRegistroDelimited(in);
        case Formato::TAMANHO:
            return lerRegistroSized(in);
    }
    throw std::runtime_error("Formato desconhecido");
}

// Funções para tamanho fixo
void Buffer::escreverRegistroFixo(const Registro& reg, std::ofstream& out) {
    std::string buffer = reg.packFixed();
    out.write(buffer.c_str(), buffer.size());
}

Registro Buffer::lerRegistroFixo(std::ifstream& in) {
    std::string buffer(100, '\0');
    in.read(&buffer[0], buffer.size());
    if (in.gcount() != static_cast<std::streamsize>(buffer.size())) {
        throw std::runtime_error("Erro ao ler registro fixo");
    }
    Registro reg;
    reg.unpackFixed(buffer);
    return reg;
}

// Funções para formato delimitado
void Buffer::escreverRegistroDelimited(const Registro& reg, std::ofstream& out) {
    out << reg.packDelimited();
}

Registro Buffer::lerRegistroDelimited(std::ifstream& in) {
    std::string buffer;
    if (!std::getline(in, buffer)) {
        throw std::runtime_error("Erro ao ler registro delimitado");
    }
    Registro reg;
    reg.unpackDelimited(buffer);
    return reg;
}

// Funções para formato com descritor de tamanho
void Buffer::escreverRegistroSized(const Registro& reg, std::ofstream& out) {
    std::string buffer = reg.packSized();
    out.write(buffer.data(), buffer.size());
}

Registro Buffer::lerRegistroSized(std::ifstream& in) {
    char sizeBuffer[sizeof(uint32_t)];
    if (!in.read(sizeBuffer, sizeof(uint32_t))) {
        throw std::runtime_error("Erro ao ler descritor de tamanho");
    }
    uint32_t size = *reinterpret_cast<uint32_t*>(sizeBuffer);
    std::string buffer(size, '\0');
    if (!in.read(&buffer[0], size)) {
        throw std::runtime_error("Erro ao ler registro com descritor de tamanho");
    }
    Registro reg;
    reg.unpackSized(buffer);
    return reg;
}
