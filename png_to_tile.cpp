#include "load_save_png.hpp"
// #include "PPU466.hpp"
#include "data_path.hpp"
#include "read_write_chunk.hpp"

#include "palettes.cpp"

#include <iostream>
#include <fstream>

// bool colors_equal(glm::u8vec4 col1, glm::)

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cout << "Error: Please provide the name of the png image and then a palette number." << std::endl;
        return 1;
    }

    // loading the png into an array of colors
    std::string png_name = argv[1];
    std::string png_path = data_path("images/" + png_name);

    // std::cout << "png path:" << png_path << std::endl;

    glm::uvec2 png_size = glm::uvec2(8, 8);
	std::vector< glm::u8vec4 > png_data;
    png_data.reserve(png_size[0] * png_size[1]);
	load_png("images/"+png_name+".png", &png_size, &png_data, UpperLeftOrigin);

    // palette extraction
    uint8_t palette_idx = argv[2][0] - '0'; // subtracting the char index of '0' to convert to int
    if (palette_idx > 7 || palette_idx < 0) {
        std::cout << "Error: Please choose a valid palette index (0 to 7)." << std::endl;
        return 1;
    }

    // loop through all pixels in a tile; check against palette colors; store as bits
    // upon inspecting outputs i'm reasonably sure this works for the test tile :D
    std::vector<uint8_t> bits0;
    bits0.resize(8);
    std::vector<uint8_t> bits1;
    bits1.resize(8);
    for (uint i = 0; i < 8; i++) {
        for (uint j = 0; j < 8; j++) {
            glm::u8vec4 color = png_data[i * 8 + j];
            uint8_t col_bit0 = 0;
            uint8_t col_bit1 = 0;
            // bit1 == 1
            if (color == game1_palettes[palette_idx][2] 
             || color == game1_palettes[palette_idx][3]) {
                col_bit1 = 1;
            }
            if (color == game1_palettes[palette_idx][1] 
             || color == game1_palettes[palette_idx][3]) {
                col_bit0 = 1;
            }
            // abort if there's a pixel color that doesn't match the palette
            if (!col_bit1 && !col_bit0 && color != game1_palettes[palette_idx][0]) {
                printf("Error: Color in pixel doesn't match any palette color\n");
                return 1;
            }
            bits1[7-i] += col_bit1 << j;
            bits0[7-i] += col_bit0 << j;
        }
    }

    // write a magic num and bits to a file
    std::filebuf fb;
    fb.open ("loadable_assets/tiles/" + png_name, std::ios::out | std::ios::binary );
    std::ostream to(&fb);
    write_chunk<uint8_t>("bit0", bits0, &to);
    write_chunk<uint8_t>("bit1", bits1, &to);
    fb.close();
}