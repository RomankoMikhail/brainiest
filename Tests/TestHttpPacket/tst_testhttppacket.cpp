#include <QtTest>

#include "HttpPacket.hpp"

class TestHttpPacket : public QObject
{
    Q_OBJECT

    class TestHeader
    {
    public:
        TestHeader(const QString &name, const QString &rawValue, const QStringList &results)
        {
            mName     = name;
            mRawValue = rawValue;
            mResults  = results;
        }

        QString name() const
        {
            return mName;
        }

        QString rawValue() const
        {
            return mRawValue;
        }

        QStringList results() const
        {
            return mResults;
        }

    private:
        QString mName;
        QString mRawValue;
        QStringList mResults;
    };

public:
private slots:
    void testHttpPacketSetField();
    void testHttpPacketSetMultipleFields();
    void testHttpPacketNameMangling();
    void testHttpPacketRealHeaders();
};

void TestHttpPacket::testHttpPacketSetField()
{
    HttpPacket packet;
    packet.setField("testName", "testValue");

    QVERIFY(packet.getValue("testName").size() == 1);
    QVERIFY(packet.getValue("testName").first() == "testValue");
}

void TestHttpPacket::testHttpPacketSetMultipleFields()
{
    const QString testString = "testValue1, testValue2,, testValue3 ";

    const QStringList results1Array({"testValue1", "testValue2", "", "testValue3"});
    const QStringList results2Array(testString);

    HttpPacket packet;

    packet.setField("testField", testString);
    packet.setField("Set-Cookie", testString);

    QVERIFY(packet.getValue("testField") == results1Array);
    QVERIFY(packet.getValue("Set-Cookie") == results2Array);
}

void TestHttpPacket::testHttpPacketNameMangling()
{
    HttpPacket packet;

    packet.setField("testfield", "1");
    packet.setField("TeStFiElD", "2");
    packet.setField("tEsTfIeLd", "3");
    packet.setField("test field", "4");
    packet.setField(" testfield", "5");
    packet.setField("testfield ", "6");

    QVERIFY(packet.getValue("testfield").size() == 1);
    QVERIFY(packet.getValue("test field").size() == 1);

    QCOMPARE(packet.getValue("testfield").first(), "6");
    QCOMPARE(packet.getValue("test field").first(), "4");
}

void TestHttpPacket::testHttpPacketRealHeaders()
{
    TestHeader host("Host", "example.com", {"example.com"});
    TestHeader userAgent(
        "User-Agent",
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:72.0) Gecko/20100101 Firefox/72.0",
        {"Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:72.0) Gecko/20100101 Firefox/72.0"});

    TestHeader accept(
        "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8",
        {"text/html", "application/xhtml+xml", "application/xml;q=0.9", "image/webp", "*/*;q=0.8"});

    TestHeader acceptLanguage("Accept-Language", "ru-RU,ru;q=0.8,en-US;q=0.5,en;q=0.3",
                              {"ru-RU", "ru;q=0.8", "en-US;q=0.5", "en;q=0.3"});

    TestHeader acceptEncoding("Accept-Encoding", "gzip, deflate, br", {"gzip", "deflate", "br"});

    TestHeader cookie("Cookie", "a=b; c=d; e=f;g=h", {"a=b; c=d; e=f;g=h"});

    TestHeader expires("Expires", "Wed, 21 Oct 2015 07:28:00 GMT",
                       {"Wed, 21 Oct 2015 07:28:00 GMT"});

    HttpPacket packet;

    packet.setField(host.name(), host.rawValue());
    packet.setField(userAgent.name(), userAgent.rawValue());
    packet.setField(accept.name(), accept.rawValue());
    packet.setField(acceptLanguage.name(), acceptLanguage.rawValue());
    packet.setField(acceptEncoding.name(), acceptEncoding.rawValue());
    packet.setField(cookie.name(), cookie.rawValue());
    packet.setField(expires.name(), expires.rawValue());

    QCOMPARE(packet.getValue(host.name()), host.results());
    QCOMPARE(packet.getValue(userAgent.name()), userAgent.results());
    QCOMPARE(packet.getValue(accept.name()), accept.results());
    QCOMPARE(packet.getValue(acceptLanguage.name()), acceptLanguage.results());
    QCOMPARE(packet.getValue(acceptEncoding.name()), acceptEncoding.results());
    QVERIFY(packet.getValue(cookie.name()) == cookie.results());
    QCOMPARE(packet.getValue(expires.name()), expires.results());
}

QTEST_APPLESS_MAIN(TestHttpPacket)

#include "tst_testhttppacket.moc"
