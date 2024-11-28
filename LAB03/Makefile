# Nome do executável
TARGET = main

# Compilador e flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -pedantic-errors
LDFLAGS =

# Arquivos de código-fonte e objetos
SRCS = main.cpp Registro.cpp Buffer.cpp
OBJS = $(SRCS:.cpp=.o)

# Regras padrão
all: $(TARGET)

# Regra para criar o executável
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Regra para criar objetos
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpeza
clean:
	rm -f $(OBJS) $(TARGET)

# Execução
run: $(TARGET)
	./$(TARGET)
