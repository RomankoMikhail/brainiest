#ifndef SOCKETCONTEXT_HPP
#define SOCKETCONTEXT_HPP

#include <HttpPacket.h>
#include <QTcpSocket>
#include <WebSocketFrame.hpp>

struct SocketContext
{
    QTcpSocket *socket;

    bool operator==(const SocketContext &other)
    {
        return other.socket == socket;
    }
};
Q_DECLARE_METATYPE(SocketContext);

using onHttpPacketFunction     = HttpPacket (*)(SocketContext &, const HttpPacket &);
using onWebsocketFrameFunction = WebSocketFrame (*)(SocketContext &, const WebSocketFrame &);

Q_DECLARE_METATYPE(onHttpPacketFunction);
Q_DECLARE_METATYPE(onWebsocketFrameFunction);

#endif // SOCKETCONTEXT_HPP
