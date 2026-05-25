#include "utils/passwordhasher.h"

#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QByteArray>

QString PasswordHasher::generateSalt(int length)
{
    const QString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    QString salt;
    salt.reserve(length);
    for (int i = 0; i < length; ++i) {
        int index = QRandomGenerator::global()->bounded(chars.size());
        salt.append(chars.at(index));
    }
    return salt;
}

QString PasswordHasher::hashPassword(const QString &password, const QString &salt)
{
    QByteArray data = (password + salt).toUtf8();
    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
    return QString::fromLatin1(hash.toHex());
}

bool PasswordHasher::verifyPassword(const QString &password, const QString &storedHash)
{
    // storedHash format: "salt:sha256hash"
    int colonIndex = storedHash.indexOf(':');
    if (colonIndex == -1) return false;

    QString salt = storedHash.left(colonIndex);
    QString hash = storedHash.mid(colonIndex + 1);

    return hashPassword(password, salt) == hash;
}
