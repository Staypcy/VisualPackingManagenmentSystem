#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include "entities/feerule.h"
#include "utils/result.h"

struct UserRecord {
    int id;
    QString username;
    QString passwordHash; // format: "salt:sha256hash"
    int role;             // 0=admin, 1=user
};

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(const QString &dbPath, QObject *parent = nullptr);
    ~DatabaseManager();

    Result<void> initialize();

    // User operations
    Result<void> createUser(const QString &username, const QString &passwordHash, int role);
    Result<UserRecord> getUser(const QString &username);
    Result<int> getUserCount();

    // Fee rule operations
    Result<FeeRule> getFeeRule();
    Result<void> saveFeeRule(const FeeRule &rule);

private:
    bool createTables();
    bool migrateFromTextFiles();

    QString m_dbPath;
    QString m_connectionName;
};

#endif // DATABASEMANAGER_H
