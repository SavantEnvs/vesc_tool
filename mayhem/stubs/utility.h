// Stand-in for vesc_tool's utility.h, which drags in the whole Qt Quick/Widgets/serial stack.
// The maddy markdown parser only calls these three statics; the theme lookup and network download are
// environment, so they are stubbed (no network, fixed colours) and everything in maddy runs for real.
#pragma once

#include <QDir>
#include <QString>
#include <QUrl>
#include <cstdint>

class Utility
{
public:
    static uint32_t crc32c(uint8_t *data, uint32_t len)
    {
        uint32_t crc = 0xFFFFFFFFu;
        for (uint32_t i = 0; i < len; i++) {
            crc ^= data[i];
            for (int k = 0; k < 8; k++) {
                crc = (crc >> 1) ^ (0x82F63B78u & (0u - (crc & 1u)));
            }
        }
        return ~crc;
    }

    static QString getAppHexColor(QString colorName)
    {
        return colorName == "lightText" ? "#000000" : "#ffffff";
    }

    static bool downloadUrlEventloop(QString, QString) { return false; }
};
