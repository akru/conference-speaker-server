#include <QString>
#include <QtTest>
#include <bandpass_filter.h>

class FiltersTests : public QObject
{
    Q_OBJECT

public:
    FiltersTests();

private Q_SLOTS:
    void firTestCase();
};

FiltersTests::FiltersTests()
{
}

void FiltersTests::firTestCase()
{
    BandpassFilter bpf;
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(FiltersTests)

#include "tst_filterstests.moc"
