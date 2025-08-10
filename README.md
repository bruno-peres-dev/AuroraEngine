# AuroraEngine

AuroraEngine é um projeto C++ com CMake, organizado em `engine/` e aplicativos em `apps/` (por exemplo, `apps/Runtime`).

## Requisitos
- CMake ≥ 3.20
- Compilador C++17+ (Visual Studio 2022 Desktop C++ ou MinGW/GCC/Clang)
- Opcional: Ninja para builds mais rápidos

## Como compilar (Windows, Visual Studio)
1. Abra um PowerShell na pasta raiz do projeto (`DE`).
2. Gere a solução:
```powershell
mkdir build
cmake -S . -B build -G "Visual Studio 17 2022"
```
3. Compile:
```powershell
cmake --build build --config Debug
```

## Como compilar (MinGW/Clang + Ninja)
```powershell
mkdir build
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## Estrutura
- `engine/`: Core, Platform, RHI e módulos relacionados
- `apps/Runtime/`: App de execução para testar a engine

## Licença
Defina uma licença (ex.: MIT) adicionando um arquivo `LICENSE` na raiz.
