#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include <QObject>
#include "services/iauthservice.h"

class DatabaseManager;

class AuthService : public QObject, public IAuthService
{
    Q_OBJECT
public:
    explicit AuthService(DatabaseManager *dbManager, QObject *parent = nullptr);
    ~AuthService() override = default;

    bool authenticate(const QString &username, const QString &password, int &role) override;
    bool registerUser(const QString &username, const QString &password, int role) override;

private:
    DatabaseManager *m_dbManager;
};

#endif // AUTHSERVICE_H
