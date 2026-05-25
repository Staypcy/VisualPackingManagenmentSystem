#ifndef PASSWORDHASHER_H
#define PASSWORDHASHER_H

#include <QString>

class PasswordHasher
{
public:
    static QString generateSalt(int length = 16);
    static QString hashPassword(const QString &password, const QString &salt);
    static bool verifyPassword(const QString &password, const QString &storedHash);
};

#endif // PASSWORDHASHER_H
