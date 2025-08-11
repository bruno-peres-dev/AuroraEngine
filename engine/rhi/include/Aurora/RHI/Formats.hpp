#pragma once

#include <cstdint>

namespace Aurora::RHI {

// Formatos de textura suportados (mínimo viável para MVP + alguns depth/FP)
enum class TextureFormat : uint8_t {
    RGBA8,
    RGB8,
    R8,
    RGBA16F,
    R16F,
    Depth24Stencil8,
    Depth32F,
};

// Filtros e endereçamento
enum class FilterMode : uint8_t { Nearest, Linear };
enum class AddressMode : uint8_t { Repeat, ClampToEdge };

// Máscara de escrita de cor
enum ColorWriteMask : uint8_t {
    ColorWrite_R = 1 << 0,
    ColorWrite_G = 1 << 1,
    ColorWrite_B = 1 << 2,
    ColorWrite_A = 1 << 3,
    ColorWrite_All = ColorWrite_R | ColorWrite_G | ColorWrite_B | ColorWrite_A
};

}


