#ifndef WEBSOCKETPARSER_H
#define WEBSOCKETPARSER_H

#include <QObject>
#include <WebSocketFrame.h>
#include <QIODevice>


class WebSocketParser : public QObject
{
    Q_OBJECT
public:
    enum State
    {
        StateReadHeader,
        StateReadPayload16Bit,
        StateReadPayload64Bit,
        StateReadMask,
        StateReadPayload
    };

    WebSocketParser(QObject *parent = nullptr);

    void parse(QIODevice *device);

signals:
    void frameReady(WebSocketFrame frame);

private:
    WebSocketFrame mCurrentFrame;

    quint64 mPayloadSize;
    quint8 mMask[4];


    State readHeader(QIODevice *device);
    State readPayloadSize16Bit(QIODevice *device);
    State readPayloadSize64Bit(QIODevice *device);
    State readMask(QIODevice *device);
    State readPayload(QIODevice *device);

    State mState = StateReadHeader;
    QByteArray mInternalBuffer;
};

#endif // WEBSOCKETPARSER_H
