#ifndef LOGGER_H
#define LOGGER_H

#include <QString>

class Logger
{
public:
    static void init(const QString &logFilePath);

private:
    Logger() = default;
};

#define LOG_DEBUG()    qDebug()
#define LOG_INFO()     qInfo()
#define LOG_WARNING()  qWarning()
#define LOG_ERROR()    qCritical()

#endif // LOGGER_H
