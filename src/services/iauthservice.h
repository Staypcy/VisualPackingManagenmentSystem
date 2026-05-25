#ifndef IAUTHSERVICE_H
#define IAUTHSERVICE_H

#include <QString>

class IAuthService
{
public:
    virtual ~IAuthService() = default;

    // Returns true if credentials match; role out-param set to 0 (admin) or 1 (user)
    virtual bool authenticate(const QString &username, const QString &password, int &role) = 0;

    virtual bool registerUser(const QString &username, const QString &password, int role) = 0;
};

#endif // IAUTHSERVICE_H
