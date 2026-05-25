#include <QtTest>
#include "database/databasemanager.h"

class DatabaseManagerTest : public QObject
{
    Q_OBJECT

private:
    DatabaseManager *dbManager;

private slots:
    void initTestCase()
    {
        dbManager = new DatabaseManager(":memory:");
        QVERIFY(dbManager->initialize().isOk());
    }

    void cleanupTestCase()
    {
        delete dbManager;
    }

    void testCreateUser()
    {
        Result<void> result = dbManager->createUser("dbuser", "salt:hash123", 1);
        QVERIFY(result.isOk());
    }

    void testCreateDuplicateUser()
    {
        dbManager->createUser("dbdup", "salt:hash", 1);
        Result<void> result = dbManager->createUser("dbdup", "salt:hash2", 1);
        QVERIFY(result.isError());
    }

    void testGetUser()
    {
        dbManager->createUser("dbget", "salt:hashvalue", 0);
        Result<UserRecord> result = dbManager->getUser("dbget");
        QVERIFY(result.isOk());
        QCOMPARE(result.value().username, "dbget");
        QCOMPARE(result.value().passwordHash, "salt:hashvalue");
        QCOMPARE(result.value().role, 0);
    }

    void testGetNonexistentUser()
    {
        Result<UserRecord> result = dbManager->getUser("no_such_user");
        QVERIFY(result.isError());
    }

    void testUserCount()
    {
        int before = dbManager->getUserCount().value();
        dbManager->createUser("count_test", "salt:hash", 1);
        int after = dbManager->getUserCount().value();
        QCOMPARE(after, before + 1);
    }

    void testGetFeeRule()
    {
        // Default rule should be present after initialization
        Result<FeeRule> result = dbManager->getFeeRule();
        QVERIFY(result.isOk());
        QCOMPARE(result.value().firstHourRate, 5.0);
        QCOMPARE(result.value().additionalHourRate, 3.0);
        QCOMPARE(result.value().dailyMax, 50.0);
        QCOMPARE(result.value().freeMinutes, 15);
    }

    void testSaveFeeRule()
    {
        FeeRule rule;
        rule.firstHourRate = 8.0;
        rule.additionalHourRate = 4.0;
        rule.dailyMax = 80.0;
        rule.freeMinutes = 20;

        Result<void> saveResult = dbManager->saveFeeRule(rule);
        QVERIFY(saveResult.isOk());

        Result<FeeRule> loadResult = dbManager->getFeeRule();
        QVERIFY(loadResult.isOk());
        QCOMPARE(loadResult.value().firstHourRate, 8.0);
        QCOMPARE(loadResult.value().additionalHourRate, 4.0);
        QCOMPARE(loadResult.value().dailyMax, 80.0);
        QCOMPARE(loadResult.value().freeMinutes, 20);
    }
};

QTEST_MAIN(DatabaseManagerTest)
#include "tst_databasemanager.moc"
