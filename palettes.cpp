#include <array>
#include <glm/glm.hpp>

// static array of color palettes :D

// pink to black fade for tiles
std::array<glm::u8vec4, 4> pink = {
     // bright pink
    glm::u8vec4(255, 90, 190, 255), 
     // darker pink
    glm::u8vec4(171, 64, 142, 255),
     // darkest pink
    glm::u8vec4(91, 53, 87, 255),
     // black
    glm::u8vec4(0, 0, 0, 255)
};

std::array<glm::u8vec4, 4> yellow = {
    // bright yellow
    glm::u8vec4(0xff, 0xa8, 0x63, 0xff),
    // medium orange-brown
    glm::u8vec4(0xbf, 0x7e, 0x2a, 0xff),
    // darker brown
    glm::u8vec4(0x79, 0x56, 0x0f, 0xff),
    // black
    glm::u8vec4(0, 0, 0, 255)
};

std::array<glm::u8vec4, 4> blue = {
    // bright
    glm::u8vec4(0x87, 0x3f, 0xff, 0xff),
    // medium
    glm::u8vec4(0x54, 0x27, 0xcf, 0xff),
    // dark
    glm::u8vec4(0x24, 0x13, 0x81, 0xff),
    // black
    glm::u8vec4(0, 0, 0, 255)
};

// transparent tiles & the player
std::array<glm::u8vec4, 4> white = {
    glm::u8vec4(255, 255, 255, 255), glm::u8vec4(0, 0, 0, 0), glm::u8vec4(0), glm::u8vec4(0, 0, 0, 0)
};

static std::array<glm::u8vec4, 4> game1_palettes[8] = 
    {pink, yellow, blue, pink, pink, pink, pink, white};