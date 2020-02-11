#include <QtTest>

// add necessary includes here

class TestCookie : public QObject
{
    Q_OBJECT

public:
    TestCookie()  = default;
    ~TestCookie() = default;

private slots:
    void test_case1();
};

void TestCookie::test_case1()
{
    QFAIL("Not implemented");
}

QTEST_APPLESS_MAIN(TestCookie)

#include "tst_testcookie.moc"
