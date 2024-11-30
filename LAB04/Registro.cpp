#include "Registro.h"
#include <sstream>
#include <iomanip>
#include <cstring>

// Tamanho fixo empacotar
std::string Registro::packFixed() const {
    char buffer[100];
    memset(buffer, '\0', sizeof(buffer));
    std::ostringstream bufferDeSaida;
    bufferDeSaida << std::left << std::setw(20) << nome
        << std::setw(20) << sobrenome
        << std::setw(15) << telefone
        << std::setw(10) << dataNascimento;
    std::string serialized = bufferDeSaida.str();
    memcpy(buffer, serialized.c_str(), std::min(serialized.size(), sizeof(buffer)));
    return std::string(buffer, sizeof(buffer));
}

// Tamanho fixo desempacotar
void Registro::unpackFixed(const std::string& buffer) {
    nome = buffer.substr(0, 20);
    sobrenome = buffer.substr(20, 20);
    telefone = buffer.substr(40, 15);
    dataNascimento = buffer.substr(55, 10);

    nome.erase(nome.find_last_not_of(' ') + 1);
    sobrenome.erase(sobrenome.find_last_not_of(' ') + 1);
    telefone.erase(telefone.find_last_not_of(' ') + 1);
    dataNascimento.erase(dataNascimento.find_last_not_of(' ') + 1);
}

// Delimitado empacotar
std::string Registro::packDelimited() const {
    std::ostringstream oss;
    oss << nome << "|" << sobrenome << "|" << telefone << "|" << dataNascimento << "\n";
    return oss.str();
}

// Delimitado desempacotar
void Registro::unpackDelimited(const std::string& buffer) {
    std::istringstream iss(buffer);
    std::getline(iss, nome, '|');
    std::getline(iss, sobrenome, '|');
    std::getline(iss, telefone, '|');
    std::getline(iss, dataNascimento, '\n');
}

// Com descritor de tamanho empacotar
std::string Registro::packSized() const {
    std::ostringstream oss;
    oss << nome << "|" << sobrenome << "|" << telefone << "|" << dataNascimento;
    std::string data = oss.str();
    uint32_t size = static_cast<uint32_t>(data.size());
    std::string packed(reinterpret_cast<char*>(&size), sizeof(size));
    return packed + data;
}

// Com descritor de tamanho desempacotar
void Registro::unpackSized(const std::string& buffer) {
    uint32_t size = *reinterpret_cast<const uint32_t*>(buffer.data());
    std::string data = buffer.substr(sizeof(size), size);
    unpackDelimited(data);
}
