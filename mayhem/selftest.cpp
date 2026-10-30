// Known-answer probe for the vesc_tool code the fuzz targets cover. Built with the project's normal
// flags by mayhem/build.sh and run by mayhem/test.sh, which compares each `KAT` line to a golden value.
#define MINIMP3_FLOAT_OUTPUT
#define MINIMP3_IMPLEMENTATION
#include "minimp3_ex.h"

#include "heatshrink/heatshrinkif.h"
#include "lzokay/lzokay.hpp"
#include "maddy/parser.h"
#include "packet.h"

#include <QByteArray>
#include <QObject>
#include <QVector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

static std::string hex(const uint8_t *p, size_t n)
{
    std::string s;
    char b[3];
    for (size_t i = 0; i < n; i++) {
        snprintf(b, sizeof(b), "%02x", p[i]);
        s += b;
    }
    return s;
}

static std::string hex(const QByteArray &a)
{
    return hex(reinterpret_cast<const uint8_t *>(a.constData()), static_cast<size_t>(a.size()));
}

namespace {
struct Sink : QObject {
    QByteArray framed;
    QVector<QByteArray> received;
    void onData(QByteArray &d) { framed = d; }
    void onPacket(QByteArray &p) { received.append(p); }
};
}

int main()
{
    // MPEG-1 Layer III, 128 kbit/s, 44.1 kHz, stereo: one silent 417-byte frame.
    {
        std::vector<uint8_t> frame(417, 0);
        frame[0] = 0xFF; frame[1] = 0xFB; frame[2] = 0x90; frame[3] = 0x00;
        mp3dec_t dec;
        mp3dec_frame_info_t info;
        static mp3d_sample_t pcm[MINIMP3_MAX_SAMPLES_PER_FRAME];
        mp3dec_init(&dec);
        int samples = mp3dec_decode_frame(&dec, frame.data(), (int)frame.size(), pcm, &info);
        printf("KAT mp3_frame samples=%d frame_bytes=%d hz=%d channels=%d layer=%d bitrate=%d\n",
               samples, info.frame_bytes, info.hz, info.channels, info.layer, info.bitrate_kbps);

        std::vector<uint8_t> two(frame);
        two.insert(two.end(), frame.begin(), frame.end());
        mp3dec_file_info_t finfo;
        mp3dec_init(&dec);
        int r = mp3dec_load_buf(&dec, two.data(), two.size(), &finfo, 0, 0);
        printf("KAT mp3_load ret=%d samples=%zu hz=%d channels=%d\n", r, finfo.samples, finfo.hz, finfo.channels);
        free(finfo.buffer);
    }

    // lzokay: deterministic LZO1X stream, and decoding it back.
    {
        std::string text(64, 'a');
        text += "abcabcabcabc";
        std::vector<uint8_t> comp(lzokay::compress_worst_size(text.size()));
        std::size_t compLen = 0;
        lzokay::EResult cr = lzokay::compress(reinterpret_cast<const uint8_t *>(text.data()), text.size(),
                                              comp.data(), comp.size(), compLen);
        printf("KAT lzo_compress ret=%d len=%zu data=%s\n", (int)cr, compLen, hex(comp.data(), compLen).c_str());
        std::vector<uint8_t> back(text.size());
        std::size_t backLen = 0;
        lzokay::EResult dr = lzokay::decompress(comp.data(), compLen, back.data(), back.size(), backLen);
        printf("KAT lzo_roundtrip ret=%d len=%zu same=%d\n", (int)dr, backLen,
               backLen == text.size() && memcmp(back.data(), text.data(), backLen) == 0);
    }

    // heatshrink: encoder output for a fixed string and the decoder restoring it.
    {
        HeatshrinkIf hs;
        QByteArray in("hello hello hello hello");
        QByteArray comp = hs.encode(in);
        QByteArray back = hs.decode(comp);
        printf("KAT heatshrink enc=%s dec=%s\n", hex(comp).c_str(), back.constData());
    }

    // maddy: markdown to HTML as Utility::md2html drives it.
    {
        std::shared_ptr<maddy::ParserConfig> config = std::make_shared<maddy::ParserConfig>();
        config->enabledParsers = maddy::types::DEFAULT;
        maddy::Parser parser(config);
        std::stringstream in("# Title\n\nSome *italic* and **bold** text.\n");
        std::string out = parser.Parse(in);
        for (char &c : out) {
            if (c == '\n') c = '|';
        }
        printf("KAT maddy %s\n", out.c_str());
    }

    // Packet: CRC16/XMODEM check value, framing, and receive path (split across two chunks).
    {
        printf("KAT crc16 %04x\n", Packet::crc16(reinterpret_cast<const unsigned char *>("123456789"), 9));

        Sink sink;
        Packet tx, rx;
        QObject::connect(&tx, &Packet::dataToSend, &sink, &Sink::onData, Qt::DirectConnection);
        QObject::connect(&rx, &Packet::packetReceived, &sink, &Sink::onPacket, Qt::DirectConnection);
        QByteArray payload("\x01\x02\x03", 3);
        tx.sendPacket(payload);
        printf("KAT packet_frame %s\n", hex(sink.framed).c_str());
        QByteArray a = sink.framed.left(4), b = sink.framed.mid(4);
        rx.processData(a);
        rx.processData(b);
        printf("KAT packet_rx count=%d payload=%s\n", (int)sink.received.size(),
               sink.received.isEmpty() ? "" : hex(sink.received[0]).c_str());
    }

    return 0;
}
