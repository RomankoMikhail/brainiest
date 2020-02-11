#ifndef WEBSOCKETPACKET_H
#define WEBSOCKETPACKET_H

#include <QByteArray>

class WebSocketFrame
{
public:
    enum WebSocketOpcode
    {
        OpcodeContinuation = 0x00,
        OpcodeText         = 0x01,
        OpcodeBinary       = 0x02,
        OpcodeClose        = 0x08,
        OpcodePing         = 0x09,
        OpcodePong         = 0x0A
    };

    WebSocketFrame();

    QByteArray toByteArray(bool useMask = false);

    const bool &isFinalFrame() const;
    void setIsFinalFrame(bool isFinalFrame);

    const QByteArray &data() const;
    void setData(const QByteArray &data);

    const WebSocketOpcode &opcode() const;
    void setOpcode(const WebSocketOpcode &opcode);

private:
    WebSocketOpcode mOpcode;
    bool mIsFinalFrame;
    QByteArray mData;
};

#endif // WEBSOCKETPACKET_H
