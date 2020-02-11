#include <QtTest>

// add necessary includes here

class TestWebSocketFrame : public QObject
{
    Q_OBJECT

public:
    TestWebSocketFrame()  = default;
    ~TestWebSocketFrame() = default;

private slots:
};

QTEST_APPLESS_MAIN(TestWebSocketFrame)

#include "tst_testwebsocketframe.moc"
