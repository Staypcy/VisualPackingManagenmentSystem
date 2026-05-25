#include "services/authservice.h"
#include "database/databasemanager.h"
#include "utils/passwordhasher.h"
#include "utils/logger.h"

AuthService::AuthService(DatabaseManager *dbManager, QObject *parent)
    : QObject(parent)
    , m_dbManager(dbManager)
{
}

bool AuthService::authenticate(const QString &username, const QString &password, int &role)
{
    Result<UserRecord> result = m_dbManager->getUser(username);
    if (result.isError()) {
        LOG_WARNING() << "Authentication failed for user" << username << ":" << result.errorMessage();
        return false;
    }

    UserRecord user = result.value();
    if (PasswordHasher::verifyPassword(password, user.passwordHash)) {
        role = user.role;
        LOG_INFO() << "User" << username << "authenticated successfully, role:" << role;
        return true;
    }

    LOG_WARNING() << "Password mismatch for user" << username;
    return false;
}

bool AuthService::registerUser(const QString &username, const QString &password, int role)
{
    // Check if user already exists
    Result<UserRecord> existing = m_dbManager->getUser(username);
    if (existing.isOk()) {
        LOG_WARNING() << "Registration failed: user" << username << "already exists";
        return false;
    }

    QString salt = PasswordHasher::generateSalt();
    QString hash = PasswordHasher::hashPassword(password, salt);
    QString storedHash = salt + ":" + hash;

    Result<void> result = m_dbManager->createUser(username, storedHash, role);
    if (result.isError()) {
        LOG_ERROR() << "Registration failed for" << username << ":" << result.errorMessage();
        return false;
    }

    LOG_INFO() << "User" << username << "registered successfully, role:" << role;
    return true;
}
