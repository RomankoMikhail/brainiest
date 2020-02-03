#include "WebSocketParser.h"
#include <QtEndian>

WebSocketParser::WebSocketParser(QObject *parent) : QObject(parent)
{
}

void WebSocketParser::parse(QIODevice *device)
{
    State previousState;

    do
    {
        previousState = mState;

        switch (mState)
        {
        case StateReadHeader:
            mState = readHeader(device);
            break;

        case StateReadPayload16Bit:
            mState = readPayloadSize16Bit(device);
            break;

        case StateReadPayload64Bit:
            mState = readPayloadSize64Bit(device);
            break;

        case StateReadMask:
            mState = readMask(device);
            break;

        case StateReadPayload:
            mState = readPayload(device);
            break;
        }

    } while (mState != previousState);
}

WebSocketParser::State WebSocketParser::readHeader(QIODevice *device)
{
    if (device->bytesAvailable() < 2)
        return StateReadHeader;

    char header[2];

    device->read(header, 2);

    mCurrentFrame = WebSocketFrame();
    mCurrentFrame.setIsFinalFrame((header[0] & 0x80) != 0);

    mCurrentFrame.setOpcode(static_cast<WebSocketFrame::WebSocketOpcode>(header[0] & 0x0F));
    mMask[0] = mMask[1] = mMask[2] = mMask[3] = (header[1] & 0x80) != 0;
    mPayloadSize                              = (header[1] & 0x7F);

    if (mPayloadSize == 126)
        return StateReadPayload16Bit;

    if (mPayloadSize == 127)
        return StateReadPayload64Bit;

    return StateReadMask;
}

WebSocketParser::State WebSocketParser::readPayloadSize16Bit(QIODevice *device)
{
    if (device->bytesAvailable() < 2)
        return StateReadPayload16Bit;

    char length[2];

    device->read(length, 2);
    mPayloadSize = qFromBigEndian<quint16>(length);

    return StateReadMask;
}

WebSocketParser::State WebSocketParser::readPayloadSize64Bit(QIODevice *device)
{
    if (device->bytesAvailable() < 2)
        return StateReadPayload64Bit;

    char length[8];

    device->read(length, 8);
    mPayloadSize = qFromBigEndian<quint64>(length);

    return StateReadMask;
}

WebSocketParser::State WebSocketParser::readMask(QIODevice *device)
{
    if (mMask[0] == 0)
        return StateReadPayload;

    if (device->bytesAvailable() < 4)
        return StateReadMask;

    device->read(reinterpret_cast<char *>(mMask), 4);

    return StateReadPayload;
}

WebSocketParser::State WebSocketParser::readPayload(QIODevice *device)
{
    if (device->bytesAvailable() < mPayloadSize)
        return StateReadPayload;

    int maskPosition = 0;

    QByteArray data;
    data.reserve(mPayloadSize);

    for (quint64 i = 0; i < mPayloadSize; i++)
    {
        char byte;
        device->read(&byte, 1);

        data.append(byte ^ mMask[maskPosition]);

        maskPosition = (maskPosition + 1) & 0x3;
    }

    mCurrentFrame.setData(data);

    QTcpSocket *socket = dynamic_cast<QTcpSocket*>(parent());

    if(socket != nullptr)
        emit frameReady(socket, mCurrentFrame);

    return StateReadHeader;
}
