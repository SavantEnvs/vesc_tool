// HeatshrinkIf is vesc_tool's wrapper around the heatshrink codec (firmware/bootloader blobs).
#include "heatshrink/heatshrinkif.h"
#include <QByteArray>
#include <cstdint>
#include <cstdlib>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < 1) {
        return -1;
    }

    static HeatshrinkIf hs;
    QByteArray in(reinterpret_cast<const char *>(data), static_cast<int>(size));

    hs.decode(in);

    if (size <= 8192) {
        QByteArray comp = hs.encode(in);
        if (hs.decode(comp) != in) {
            abort();
        }
    }
    return 0;
}
