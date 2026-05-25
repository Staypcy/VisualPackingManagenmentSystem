#include "database/databasemanager.h"
#include "utils/logger.h"
#include "utils/passwordhasher.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSqlError>
#include <QSqlQuery>
#include <QTextStream>

DatabaseManager::DatabaseManager(const QString &dbPath, QObject *parent)
    : QObject(parent)
    , m_dbPath(dbPath)
    , m_connectionName("vpms_conn_" + QString::number(reinterpret_cast<quintptr>(this)))
{
}

DatabaseManager::~DatabaseManager()
{
    if (QSqlDatabase::contains(m_connectionName)) {
        QSqlDatabase::database(m_connectionName).close();
        QSqlDatabase::removeDatabase(m_connectionName);
    }
}

Result<void> DatabaseManager::initialize()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    db.setDatabaseName(m_dbPath);

    if (!db.open()) {
        LOG_ERROR() << "Failed to open database:" << db.lastError().text();
        return Result<void>::error("无法打开数据库: " + db.lastError().text());
    }

    if (!createTables()) {
        return Result<void>::error("无法创建数据库表");
    }

    if (!migrateFromTextFiles()) {
        LOG_WARNING() << "Text file migration encountered issues, continuing anyway";
    }

    LOG_INFO() << "Database initialized successfully at" << m_dbPath;
    return Result<void>::ok();
}

bool DatabaseManager::createTables()
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query(db);

    const QString createUsers = R"(
        CREATE TABLE IF NOT EXISTS users (
            id              INTEGER PRIMARY KEY AUTOINCREMENT,
            username        TEXT NOT NULL UNIQUE,
            password_hash   TEXT NOT NULL,
            role            INTEGER NOT NULL,
            created_at      TEXT NOT NULL DEFAULT (datetime('now')),
            updated_at      TEXT NOT NULL DEFAULT (datetime('now'))
        )
    )";

    const QString createFeeRules = R"(
        CREATE TABLE IF NOT EXISTS fee_rules (
            id                  INTEGER PRIMARY KEY CHECK (id = 1),
            first_hour_rate     REAL NOT NULL DEFAULT 5.0,
            additional_hour_rate REAL NOT NULL DEFAULT 3.0,
            daily_max           REAL NOT NULL DEFAULT 50.0,
            free_minutes        INTEGER NOT NULL DEFAULT 15,
            updated_at          TEXT NOT NULL DEFAULT (datetime('now'))
        )
    )";

    const QString createVehicles = R"(
        CREATE TABLE IF NOT EXISTS vehicles (
            id              INTEGER PRIMARY KEY AUTOINCREMENT,
            plate_number    TEXT NOT NULL,
            vehicle_type    TEXT,
            color           TEXT,
            image_path      TEXT,
            entry_time      TEXT NOT NULL,
            exit_time       TEXT,
            fee             REAL DEFAULT 0.0,
            created_at      TEXT NOT NULL DEFAULT (datetime('now'))
        )
    )";

    const QString createConfig = R"(
        CREATE TABLE IF NOT EXISTS config (
            key     TEXT PRIMARY KEY,
            value   TEXT NOT NULL
        )
    )";

    if (!query.exec(createUsers)) {
        LOG_ERROR() << "Failed to create users table:" << query.lastError().text();
        return false;
    }
    if (!query.exec(createFeeRules)) {
        LOG_ERROR() << "Failed to create fee_rules table:" << query.lastError().text();
        return false;
    }
    if (!query.exec(createVehicles)) {
        LOG_ERROR() << "Failed to create vehicles table:" << query.lastError().text();
        return false;
    }
    if (!query.exec(createConfig)) {
        LOG_ERROR() << "Failed to create config table:" << query.lastError().text();
        return false;
    }

    // Insert default fee rule if not exists
    query.exec("INSERT OR IGNORE INTO fee_rules (id, first_hour_rate, additional_hour_rate, daily_max, free_minutes) "
               "VALUES (1, 5.0, 3.0, 50.0, 15)");

    return true;
}

bool DatabaseManager::migrateFromTextFiles()
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query(db);

    // Migrate users from name_password.txt
    Result<int> userCount = getUserCount();
    if (userCount.isOk() && userCount.value() == 0) {
        QString passwordFile = QCoreApplication::applicationDirPath() + "/name_password.txt";
        QFile file(passwordFile);
        if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            LOG_INFO() << "Migrating users from name_password.txt...";
            QTextStream in(&file);
            while (!in.atEnd()) {
                QString line = in.readLine().trimmed();
                if (line.isEmpty()) continue;

                QStringList parts = line.split(' ');
                if (parts.size() >= 3) {
                    int role = parts[0].toInt();
                    QString username = parts[1];
                    QString password = parts[2];

                    QString salt = PasswordHasher::generateSalt();
                    QString hash = PasswordHasher::hashPassword(password, salt);
                    QString storedHash = salt + ":" + hash;

                    query.prepare("INSERT INTO users (username, password_hash, role) VALUES (?, ?, ?)");
                    query.addBindValue(username);
                    query.addBindValue(storedHash);
                    query.addBindValue(role);
                    if (!query.exec()) {
                        LOG_WARNING() << "Failed to migrate user" << username << ":" << query.lastError().text();
                    } else {
                        LOG_INFO() << "Migrated user:" << username << "role:" << role;
                    }
                }
            }
            file.close();

            // Rename original file to .bak
            QString backupPath = passwordFile + ".bak";
            QFile::remove(backupPath);
            QFile::rename(passwordFile, backupPath);
            LOG_INFO() << "name_password.txt migrated and renamed to .bak";
        }
    }

    // Migrate fee rules from fee_rule.txt
    query.exec("SELECT COUNT(*) FROM fee_rules");
    if (query.next() && query.value(0).toInt() > 0) {
        QString feeFile = QCoreApplication::applicationDirPath() + "/fee_rule.txt";
        QFile file(feeFile);
        if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            LOG_INFO() << "Migrating fee rules from fee_rule.txt...";

            FeeRule rule;
            QTextStream in(&file);
            while (!in.atEnd()) {
                QString line = in.readLine().trimmed();
                QStringList parts = line.split("=");
                if (parts.size() == 2) {
                    QString key = parts[0].trimmed();
                    double value = parts[1].trimmed().toDouble();

                    if (key == "firstHourRate") rule.firstHourRate = value;
                    else if (key == "additionalHourRate") rule.additionalHourRate = value;
                    else if (key == "dailyMax") rule.dailyMax = value;
                    else if (key == "freeMinutes") rule.freeMinutes = static_cast<int>(value);
                }
            }
            file.close();

            saveFeeRule(rule);

            QString backupPath = feeFile + ".bak";
            QFile::remove(backupPath);
            QFile::rename(feeFile, backupPath);
            LOG_INFO() << "fee_rule.txt migrated and renamed to .bak";
        }
    }

    return true;
}

Result<void> DatabaseManager::createUser(const QString &username, const QString &passwordHash, int role)
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query(db);
    query.prepare("INSERT INTO users (username, password_hash, role) VALUES (?, ?, ?)");
    query.addBindValue(username);
    query.addBindValue(passwordHash);
    query.addBindValue(role);

    if (!query.exec()) {
        LOG_ERROR() << "Failed to create user:" << query.lastError().text();
        return Result<void>::error("创建用户失败: " + query.lastError().text());
    }
    return Result<void>::ok();
}

Result<UserRecord> DatabaseManager::getUser(const QString &username)
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query(db);
    query.prepare("SELECT id, username, password_hash, role FROM users WHERE username = ?");
    query.addBindValue(username);

    if (!query.exec()) {
        return Result<UserRecord>::error("查询用户失败: " + query.lastError().text());
    }

    if (query.next()) {
        UserRecord user;
        user.id = query.value(0).toInt();
        user.username = query.value(1).toString();
        user.passwordHash = query.value(2).toString();
        user.role = query.value(3).toInt();
        return Result<UserRecord>::ok(user);
    }

    return Result<UserRecord>::error("用户不存在");
}

Result<int> DatabaseManager::getUserCount()
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query(db);
    if (!query.exec("SELECT COUNT(*) FROM users")) {
        return Result<int>::error("无法查询用户数量");
    }
    if (query.next()) {
        return Result<int>::ok(query.value(0).toInt());
    }
    return Result<int>::ok(0);
}

Result<FeeRule> DatabaseManager::getFeeRule()
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query(db);
    if (!query.exec("SELECT first_hour_rate, additional_hour_rate, daily_max, free_minutes FROM fee_rules WHERE id = 1")) {
        return Result<FeeRule>::error("查询计费规则失败: " + query.lastError().text());
    }

    if (query.next()) {
        FeeRule rule;
        rule.firstHourRate = query.value(0).toDouble();
        rule.additionalHourRate = query.value(1).toDouble();
        rule.dailyMax = query.value(2).toDouble();
        rule.freeMinutes = query.value(3).toInt();
        return Result<FeeRule>::ok(rule);
    }

    // Return default if no row exists
    return Result<FeeRule>::ok(FeeRule());
}

Result<void> DatabaseManager::saveFeeRule(const FeeRule &rule)
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query(db);
    query.prepare("UPDATE fee_rules SET first_hour_rate = ?, additional_hour_rate = ?, daily_max = ?, free_minutes = ?, updated_at = datetime('now') WHERE id = 1");
    query.addBindValue(rule.firstHourRate);
    query.addBindValue(rule.additionalHourRate);
    query.addBindValue(rule.dailyMax);
    query.addBindValue(rule.freeMinutes);

    if (!query.exec()) {
        LOG_ERROR() << "Failed to save fee rule:" << query.lastError().text();
        return Result<void>::error("保存计费规则失败: " + query.lastError().text());
    }
    return Result<void>::ok();
}
