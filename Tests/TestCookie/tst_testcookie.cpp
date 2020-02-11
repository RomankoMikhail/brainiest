#include <QtTest>

// add necessary includes here

class TestCookie : public QObject
{
    Q_OBJECT

public:
    TestCookie()  = default;
    ~TestCookie() = default;

private slots:
};

QTEST_APPLESS_MAIN(TestCookie)

#include "tst_testcookie.moc"
