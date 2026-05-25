#include <QtTest>
#include "services/authservice.h"
#include "database/databasemanager.h"
#include "utils/passwordhasher.h"

class AuthServiceTest : public QObject
{
    Q_OBJECT

private:
    DatabaseManager *dbManager;
    AuthService *authService;

private slots:
    void initTestCase()
    {
        dbManager = new DatabaseManager(":memory:");
        QVERIFY(dbManager->initialize().isOk());
        authService = new AuthService(dbManager);
    }

    void cleanupTestCase()
    {
        delete authService;
        delete dbManager;
    }

    void testRegisterUser()
    {
        bool ok = authService->registerUser("testuser", "password123", 1);
        QVERIFY(ok);
    }

    void testRegisterDuplicateUser()
    {
        authService->registerUser("duplicate", "pass1", 1);
        bool ok = authService->registerUser("duplicate", "pass2", 1);
        QVERIFY(!ok);
    }

    void testAuthenticateSuccess()
    {
        authService->registerUser("authuser", "correct", 1);
        int role = -1;
        bool ok = authService->authenticate("authuser", "correct", role);
        QVERIFY(ok);
        QCOMPARE(role, 1);
    }

    void testAuthenticateWrongPassword()
    {
        authService->registerUser("wrongpw", "realpass", 1);
        int role = -1;
        bool ok = authService->authenticate("wrongpw", "badpass", role);
        QVERIFY(!ok);
    }

    void testAuthenticateNonexistentUser()
    {
        int role = -1;
        bool ok = authService->authenticate("noone", "password", role);
        QVERIFY(!ok);
    }

    void testAdminRole()
    {
        authService->registerUser("admin", "admin123", 0);
        int role = -1;
        bool ok = authService->authenticate("admin", "admin123", role);
        QVERIFY(ok);
        QCOMPARE(role, 0);
    }

    void testPasswordIsHashed()
    {
        authService->registerUser("hashcheck", "secret", 1);
        Result<UserRecord> result = dbManager->getUser("hashcheck");
        QVERIFY(result.isOk());
        QString storedHash = result.value().passwordHash;

        // Should contain colon separator
        QVERIFY(storedHash.contains(':'));
        // Should NOT contain the plaintext password
        QVERIFY(!storedHash.contains("secret"));
    }
};

QTEST_MAIN(AuthServiceTest)
#include "tst_authservice.moc"
