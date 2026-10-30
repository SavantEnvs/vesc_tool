// lzokay is what ConfigParams::loadCompressed uses to unpack base64'd LZO XML configs.
#include "lzokay/lzokay.hpp"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < 3) {
        return -1;
    }

    // First two bytes size the destination so an overrun is caught at its exact bound.
    const std::size_t dstSize = 1 + (std::size_t(data[0]) | std::size_t(data[1]) << 8);
    const uint8_t *src = data + 2;
    const std::size_t srcSize = size - 2;

    {
        std::vector<uint8_t> dst(dstSize);
        std::size_t outLen = 0;
        lzokay::decompress(src, srcSize, dst.data(), dst.size(), outLen);
    }

    // Compress/decompress round trip must be lossless.
    if (srcSize <= 4096) {
        std::vector<uint8_t> comp(lzokay::compress_worst_size(srcSize));
        std::size_t compLen = 0;
        if (lzokay::compress(src, srcSize, comp.data(), comp.size(), compLen) ==
            lzokay::EResult::Success) {
            std::vector<uint8_t> back(srcSize + 1);
            std::size_t backLen = 0;
            if (lzokay::decompress(comp.data(), compLen, back.data(), back.size(), backLen) !=
                    lzokay::EResult::Success ||
                backLen != srcSize || std::memcmp(back.data(), src, srcSize) != 0) {
                abort();
            }
        }
    }
    return 0;
}
