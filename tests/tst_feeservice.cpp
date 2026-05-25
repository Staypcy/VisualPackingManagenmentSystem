#include <QtTest>
#include "services/feeservice.h"
#include "database/databasemanager.h"
#include <QCoreApplication>

class FeeServiceTest : public QObject
{
    Q_OBJECT

private:
    DatabaseManager *dbManager;
    FeeService *feeService;

private slots:
    void initTestCase()
    {
        // Use in-memory database for testing
        dbManager = new DatabaseManager(":memory:");
        QVERIFY(dbManager->initialize().isOk());
        feeService = new FeeService(dbManager);
    }

    void cleanupTestCase()
    {
        delete feeService;
        delete dbManager;
    }

    void testDefaultFeeRule()
    {
        FeeRule rule = feeService->getRule();
        QCOMPARE(rule.firstHourRate, 5.0);
        QCOMPARE(rule.additionalHourRate, 3.0);
        QCOMPARE(rule.dailyMax, 50.0);
        QCOMPARE(rule.freeMinutes, 15);
    }

    void testFreePeriod()
    {
        QDateTime enter = QDateTime::currentDateTime();
        QDateTime exit = enter.addSecs(10 * 60); // 10 minutes, under 15 min free
        double fee = feeService->calculateFee(enter, exit);
        QCOMPARE(fee, 0.0);
    }

    void testExactlyFreeLimit()
    {
        QDateTime enter = QDateTime::currentDateTime();
        QDateTime exit = enter.addSecs(15 * 60); // exactly 15 min
        double fee = feeService->calculateFee(enter, exit);
        QCOMPARE(fee, 0.0);
    }

    void testFirstHour()
    {
        QDateTime enter = QDateTime::currentDateTime();
        QDateTime exit = enter.addSecs(30 * 60); // 30 minutes
        double fee = feeService->calculateFee(enter, exit);
        QCOMPARE(fee, 5.0);
    }

    void testTwoHours()
    {
        QDateTime enter = QDateTime::currentDateTime();
        QDateTime exit = enter.addSecs(120 * 60); // 2 hours
        double fee = feeService->calculateFee(enter, exit);
        QCOMPARE(fee, 8.0); // 5 + 3
    }

    void testDailyMax()
    {
        QDateTime enter = QDateTime::currentDateTime();
        QDateTime exit = enter.addSecs(24 * 3600); // 24 hours
        double fee = feeService->calculateFee(enter, exit);
        QVERIFY(fee <= 50.0);
        QCOMPARE(fee, 50.0);
    }

    void testNegativeDuration()
    {
        QDateTime enter = QDateTime::currentDateTime();
        QDateTime exit = enter.addSecs(-3600); // exit before enter
        double fee = feeService->calculateFee(enter, exit);
        QCOMPARE(fee, 0.0);
    }

    void testSetAndGetRule()
    {
        FeeRule newRule;
        newRule.firstHourRate = 10.0;
        newRule.additionalHourRate = 5.0;
        newRule.dailyMax = 100.0;
        newRule.freeMinutes = 30;

        feeService->setRule(newRule);
        FeeRule retrieved = feeService->getRule();

        QCOMPARE(retrieved.firstHourRate, 10.0);
        QCOMPARE(retrieved.additionalHourRate, 5.0);
        QCOMPARE(retrieved.dailyMax, 100.0);
        QCOMPARE(retrieved.freeMinutes, 30);
    }

    void testCustomRuleCalculation()
    {
        FeeRule customRule;
        customRule.firstHourRate = 10.0;
        customRule.additionalHourRate = 2.0;
        customRule.dailyMax = 60.0;
        customRule.freeMinutes = 0;
        feeService->setRule(customRule);

        QDateTime enter = QDateTime::currentDateTime();
        QDateTime exit = enter.addSecs(90 * 60); // 1.5 hours
        double fee = feeService->calculateFee(enter, exit);
        QCOMPARE(fee, 12.0); // 10 + 2
    }
};

QTEST_MAIN(FeeServiceTest)
#include "tst_feeservice.moc"
