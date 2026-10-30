// Packet is the VESC wire framing (start byte, length, payload, CRC16, 0x03) used on every
// transport (serial, BLE, TCP, UDP). Bit 0 of the first byte selects the mode:
//   0: the rest is a raw byte stream fed to the receiver (split in two to exercise the state machine)
//   1: the rest is a payload that is framed with sendPacket() and must round-trip through the receiver
#include "packet.h"
#include <QByteArray>
#include <QObject>
#include <cstdint>
#include <cstdlib>

namespace {
struct Sink : QObject {
    QByteArray framed;
    QVector<QByteArray> received;
    void onData(QByteArray &d) { framed = d; }
    void onPacket(QByteArray &p) { received.append(p); }
};
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < 2) {
        return -1;
    }

    Sink sink;
    Packet rx;
    QObject::connect(&rx, &Packet::packetReceived, &sink, &Sink::onPacket, Qt::DirectConnection);

    const char *body = reinterpret_cast<const char *>(data + 1);
    const int bodyLen = static_cast<int>(size - 1);

    if (data[0] & 1) {
        Packet tx;
        QObject::connect(&tx, &Packet::dataToSend, &sink, &Sink::onData, Qt::DirectConnection);
        QByteArray payload(body, bodyLen);
        tx.sendPacket(payload);
        if (sink.framed.isEmpty()) {
            return 0;
        }
        rx.processData(sink.framed);
        if (sink.received.size() != 1 || sink.received[0] != payload) {
            abort();
        }
    } else {
        const int split = bodyLen / 2;
        rx.processData(QByteArray(body, split));
        rx.processData(QByteArray(body + split, bodyLen - split));
    }
    return 0;
}
