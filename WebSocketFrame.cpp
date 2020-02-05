#include "WebSocketFrame.hpp"
#include <QDataStream>
#include <QRandomGenerator>

WebSocketFrame::WebSocketFrame()
{
}

QByteArray WebSocketFrame::toByteArray(bool useMask)
{
    QByteArray array;
    QDataStream dataStream(&array, QIODevice::WriteOnly);
    dataStream.setByteOrder(QDataStream::BigEndian);

    quint8 header[2] = {0, 0};

    header[0] = static_cast<char>(isFinalFrame()) << 7 | (static_cast<char>(opcode()) & 0x0F);

    if (useMask)
        header[1] = 1 << 7;

    dataStream << header[0];

    if (mData.size() <= 125)
    {
        header[1] |= mData.size() & 0x7F;
        dataStream << header[1];
    }
    else if (mData.size() <= 65535)
    {
        header[1] |= 126;
        dataStream << header[1];
        dataStream << static_cast<quint16>(mData.size());
    }
    else
    {
        header[1] |= 127;
        dataStream << header[1];
        dataStream << static_cast<quint64>(mData.size());
    }

    quint32 mask = 0;

    if (useMask)
    {
        mask = QRandomGenerator().generate();
        dataStream << mask;
    }

    int maskPosition = 0;
    quint8 *byteMask = reinterpret_cast<quint8 *>(&mask);

    for (auto byte : mData)
    {
        array.push_back(byte ^ byteMask[maskPosition]);

        maskPosition = (maskPosition + 1) & 0x3;
    }

    return array;
}

bool WebSocketFrame::isFinalFrame() const
{
    return mIsFinalFrame;
}

void WebSocketFrame::setIsFinalFrame(bool isFinalFrame)
{
    mIsFinalFrame = isFinalFrame;
}

QByteArray WebSocketFrame::data() const
{
    return mData;
}

void WebSocketFrame::setData(const QByteArray &data)
{
    mData = data;
}

WebSocketFrame::WebSocketOpcode WebSocketFrame::opcode() const
{
    return mOpcode;
}

void WebSocketFrame::setOpcode(const WebSocketOpcode &opcode)
{
    mOpcode = opcode;
}
