# Nome do executável
EXEC     := main
SRCS     := main.cpp BTree.cpp
OBJS     := $(SRCS:.cpp=.o)

# Configuração do compilador
CXX      := g++
CFLAGS   := -Wall -Wextra -std=c++17 -pedantic-errors -g
LDFLAGS  := 

# Detecta sistema operacional para definir extensão do executável
UNAME := $(shell uname -s)
ifeq ($(UNAME), Windows_NT)
    EXECUTABLE := $(EXEC).exe
    RM         := del /Q
else
    EXECUTABLE := $(EXEC).out
    RM         := rm -rf
endif

# Regra principal
all: $(EXECUTABLE)
	@echo "Compilação concluída!"

$(EXECUTABLE): $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

# Limpeza dos arquivos compilados
clean:
	$(RM) $(OBJS) $(EXECUTABLE)
	rm -rf btree.txt btree.bin
	@echo "Cleanup concluído!"

# Execução do programa após a compilação
run: all
	./$(EXECUTABLE)
	@echo "Execução concluída!"

# Regra para compilação paralela (para Linux/macOS)
parallel:
	$(MAKE) -j$(shell nproc)

