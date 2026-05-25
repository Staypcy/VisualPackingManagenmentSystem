#include <QtTest>
#include "utils/passwordhasher.h"

class PasswordHasherTest : public QObject
{
    Q_OBJECT

private slots:
    void testGenerateSalt()
    {
        QString salt1 = PasswordHasher::generateSalt();
        QString salt2 = PasswordHasher::generateSalt();
        QCOMPARE(salt1.size(), 16);
        QVERIFY(salt1 != salt2); // should be random
    }

    void testGenerateSaltCustomLength()
    {
        QString salt = PasswordHasher::generateSalt(32);
        QCOMPARE(salt.size(), 32);
    }

    void testHashPassword()
    {
        QString hash = PasswordHasher::hashPassword("test123", "abcd1234");
        QCOMPARE(hash.size(), 64); // SHA-256 produces 64 hex chars
        QVERIFY(!hash.isEmpty());
    }

    void testHashIsDeterministic()
    {
        QString hash1 = PasswordHasher::hashPassword("test123", "salt");
        QString hash2 = PasswordHasher::hashPassword("test123", "salt");
        QCOMPARE(hash1, hash2);
    }

    void testDifferentPasswordsProduceDifferentHashes()
    {
        QString hash1 = PasswordHasher::hashPassword("password1", "salt");
        QString hash2 = PasswordHasher::hashPassword("password2", "salt");
        QVERIFY(hash1 != hash2);
    }

    void testDifferentSaltsProduceDifferentHashes()
    {
        QString hash1 = PasswordHasher::hashPassword("test123", "salt1");
        QString hash2 = PasswordHasher::hashPassword("test123", "salt2");
        QVERIFY(hash1 != hash2);
    }

    void testVerifyPassword()
    {
        QString salt = PasswordHasher::generateSalt();
        QString hash = PasswordHasher::hashPassword("myPassword", salt);
        QString stored = salt + ":" + hash;

        QVERIFY(PasswordHasher::verifyPassword("myPassword", stored));
        QVERIFY(!PasswordHasher::verifyPassword("wrongPassword", stored));
    }

    void testVerifyInvalidFormat()
    {
        QVERIFY(!PasswordHasher::verifyPassword("test", "invalidformat"));
    }
};

QTEST_MAIN(PasswordHasherTest)
#include "tst_passwordhasher.moc"
