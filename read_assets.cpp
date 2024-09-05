#include "read_assets.hpp"

#include <iostream>
#include <fstream>

PPU466::Tile read_tile(const std::string fname) {
    std::filebuf fb;
    fb.open("loadable_assets/tiles/" + fname, std::ios::in | std::ios::binary);
    std::istream from(&fb);
    std::vector<uint8_t> bit0;
    bit0.resize(8);
    std::vector<uint8_t> bit1;
    bit1.resize(8);
    read_chunk<uint8_t>(from, "bit0", &bit0);
    read_chunk<uint8_t>(from, "bit1", &bit1);
    PPU466::Tile ret;
    std::array<uint8_t, 8> bit0_arr;
    std::array<uint8_t, 8> bit1_arr;

    // idea to use copy_n from:
    // https://stackoverflow.com/questions/21276889/copy-stdvector-into-stdarray
    std::copy_n(std::make_move_iterator(bit0.begin()), 8, bit0_arr.begin());
    std::copy_n(std::make_move_iterator(bit1.begin()), 8, bit1_arr.begin());
   
   ret.bit0 = bit0_arr;
   ret.bit1 = bit1_arr;
   return ret;
}