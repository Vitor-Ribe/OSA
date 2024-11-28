#include "Buffer.h"

Buffer::Buffer(const std::string& nomeArquivo, std::ios_base::openmode modo){
    arquivo.open(nomeArquivo, modo);
    if (!arquivo.is_open()){
        throw std::runtime_error("Não foi possível abrir o arquivo.");
    }
}

Buffer::~Buffer(){
    if (arquivo.is_open()){
        arquivo.close();
    }
}

void Buffer::escreverRegistro(Registro& reg){
    char buffer[54];
    reg.serializar(buffer);
    arquivo.write(reinterpret_cast<char*>(buffer), sizeof(buffer));
}

bool Buffer::lerRegistro(Registro& reg){
    char buffer[54];
    if (arquivo.read(reinterpret_cast<char*>(buffer), sizeof(buffer))){
        reg.desserializar(buffer);
        return true; 
    }
    return false;
}
