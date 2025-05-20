#include "sha256.h"
#include <cstring>
#include <sstream>
#include <iomanip>
#include <vector>
#include <array>
#include <cstdint>

// Constants from the SHA-256 spec
namespace {
    constexpr std::array<uint32_t,64> K = {
      0x428a2f98ul,0x71374491ul,0xb5c0fbcful,0xe9b5dba5ul,0x3956c25bul,0x59f111f1ul,
      0x923f82a4ul,0xab1c5ed5ul,0xd807aa98ul,0x12835b01ul,0x243185beul,0x550c7dc3ul,
      0x72be5d74ul,0x80deb1feul,0x9bdc06a7ul,0xc19bf174ul,0xe49b69c1ul,0xefbe4786ul,
      0x0fc19dc6ul,0x240ca1ccul,0x2de92c6ful,0x4a7484aaul,0x5cb0a9dcul,0x76f988daul,
      0x983e5152ul,0xa831c66dul,0xb00327c8ul,0xbf597fc7ul,0xc6e00bf3ul,0xd5a79147ul,
      0x06ca6351ul,0x14292967ul,0x27b70a85ul,0x2e1b2138ul,0x4d2c6dfcul,0x53380d13ul,
      0x650a7354ul,0x766a0abbul,0x81c2c92eul,0x92722c85ul,0xa2bfe8a1ul,0xa81a664bul,
      0xc24b8b70ul,0xc76c51a3ul,0xd192e819ul,0xd6990624ul,0xf40e3585ul,0x106aa070ul,
      0x19a4c116ul,0x1e376c08ul,0x2748774cul,0x34b0bcb5ul,0x391c0cb3ul,0x4ed8aa4aul,
      0x5b9cca4ful,0x682e6ff3ul,0x748f82eeul,0x78a5636ful,0x84c87814ul,0x8cc70208ul,
      0x90befffaul,0xa4506cebul,0xbef9a3f7ul,0xc67178f2ul
    };

// Right‐rotate
inline uint32_t rotr(uint32_t x, uint32_t n) {
    return (x >> n) | (x << (32 - n));
}
} // anon

std::string sha256(const std::string& input) {
    // Initial hash state
    uint32_t h[8] = {
      0x6a09e667ul,0xbb67ae85ul,0x3c6ef372ul,0xa54ff53aul,
      0x510e527ful,0x9b05688cul,0x1f83d9abul,0x5be0cd19ul
    };

    // Pre‐process: copy input bytes, append 0x80, pad to ≡56 mod 64, append 64-bit len
    std::vector<uint8_t> data(input.begin(), input.end());
    uint64_t bit_len = uint64_t(data.size()) * 8;
    data.push_back(0x80);
    while (data.size() % 64 != 56) data.push_back(0);
    for (int i = 7; i >= 0; --i) {
        data.push_back((bit_len >> (i * 8)) & 0xFF);
    }

    // Process each 512-bit chunk
    for (size_t off = 0; off < data.size(); off += 64) {
        uint32_t w[64];
        // Build message schedule
        for (int i = 0; i < 16; ++i) {
            w[i] = (data[off+4*i] <<24)|(data[off+4*i+1]<<16)
                 |(data[off+4*i+2]<<8)|(data[off+4*i+3]);
        }
        for (int i = 16; i < 64; ++i) {
            uint32_t s0 = rotr(w[i-15],7)^rotr(w[i-15],18)^(w[i-15]>>3);
            uint32_t s1 = rotr(w[i-2],17)^rotr(w[i-2],19)^(w[i-2]>>10);
            w[i] = w[i-16] + s0 + w[i-7] + s1;
        }

        // Initialize working vars
        uint32_t a=h[0], b=h[1], c=h[2], d=h[3],
                 e=h[4], f=h[5], g=h[6], hh=h[7];

        // Compression loop
        for (int i = 0; i < 64; ++i) {
            uint32_t S1 = rotr(e,6)^rotr(e,11)^rotr(e,25);
            uint32_t ch = (e & f) ^ ((~e) & g);
            uint32_t temp1 = hh + S1 + ch + K[i] + w[i];
            uint32_t S0 = rotr(a,2)^rotr(a,13)^rotr(a,22);
            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            uint32_t temp2 = S0 + maj;
            hh=g; g=f; f=e; e=d+temp1;
            d=c; c=b; b=a; a=temp1+temp2;
        }

        // Add back to state
        h[0]+=a; h[1]+=b; h[2]+=c; h[3]+=d;
        h[4]+=e; h[5]+=f; h[6]+=g; h[7]+=hh;
    }

    // Produce hex string
    std::ostringstream oss;
    oss<<std::hex<<std::setfill('0');
    for (auto v : h) oss<<std::setw(8)<<v;
    return oss.str();
}
