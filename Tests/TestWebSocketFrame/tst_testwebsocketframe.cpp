#include <QtTest>

// add necessary includes here

class TestWebSocketFrame : public QObject
{
    Q_OBJECT

public:
    TestWebSocketFrame() = default;
    ~TestWebSocketFrame() = default;

private slots:
    void test_case1();
};

void TestWebSocketFrame::test_case1()
{
    QFAIL("Not implemented");
}

QTEST_APPLESS_MAIN(TestWebSocketFrame)

#include "tst_testwebsocketframe.moc"
