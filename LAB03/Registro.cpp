#include "Registro.h"

std::string Registro::serializarDelimitadores() const {
    std::ostringstream oss;
    oss << nome << "|" << sobrenome << "|" << telefone << "|" << dataNascimento << "\n";
    return oss.str();
}

std::string Registro::serializarDescritor() const {
    std::ostringstream oss;
    std::string dados = nome + "|" + sobrenome + "|" + telefone + "|" + dataNascimento;
    size_t tamanho = dados.size();
    oss.write(reinterpret_cast<const char*>(&tamanho), sizeof(size_t));
    oss << dados;
    return oss.str();
}

void Registro::desserializarDelimitadores(const std::string& linha) {
    std::istringstream iss(linha);
    std::getline(iss, nome, '|');
    std::getline(iss, sobrenome, '|');
    std::getline(iss, telefone, '|');
    std::getline(iss, dataNascimento, '\n');
}

void Registro::desserializarDescritor(const std::string& buffer) {
    std::istringstream iss(buffer);
    size_t tamanho;
    iss.read(reinterpret_cast<char*>(&tamanho), sizeof(size_t));
    std::string dados(tamanho, '\0');
    iss.read(&dados[0], tamanho);

    std::istringstream dadosStream(dados);
    std::getline(dadosStream, nome, '|');
    std::getline(dadosStream, sobrenome, '|');
    std::getline(dadosStream, telefone, '|');
    std::getline(dadosStream, dataNascimento, '\n');
}