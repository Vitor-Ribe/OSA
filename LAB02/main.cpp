#include "Arquivo.h"
#include <iostream>

int main(){
    Arquivo arquivo;

    arquivo.carregarCSV("dados.csv");
    arquivo.salvarRegistrosBinario("saida.bin");
    
    Buffer buffer("saida.bin", std::ios::in | std::ios::binary);
    Registro reg;

    std::cout << "Registros lidos do arquivo binário:" << std::endl;
    while (buffer.lerRegistro(reg)) {
        std::cout << "Nome: " << reg.nome << ", Idade: " << reg.idade << std::endl;
    }
    return 0;
}
