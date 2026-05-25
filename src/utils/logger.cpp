#include "utils/logger.h"

#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QMutex>
#include <QTextStream>
#include <QCoreApplication>
#include <QDir>

static QMutex s_logMutex;
static QString s_logFilePath;
static const qint64 s_maxFileSize = 10 * 1024 * 1024; // 10MB
static const int s_maxBackupCount = 5;

static QtMessageHandler s_oldHandler = nullptr;

static void rotateLogFile()
{
    QString basePath = s_logFilePath;
    if (basePath.isEmpty()) return;

    // Rotate: vpms.log → vpms.1.log → vpms.2.log → ... → vpms.5.log (oldest deleted)
    for (int i = s_maxBackupCount - 1; i >= 0; --i) {
        QString oldFile = (i == 0) ? basePath : basePath + "." + QString::number(i);
        QString newFile = basePath + "." + QString::number(i + 1);

        if (QFile::exists(oldFile)) {
            if (i == s_maxBackupCount - 1) {
                QFile::remove(oldFile);
            } else {
                QFile::remove(newFile);
                QFile::rename(oldFile, newFile);
            }
        }
    }
}

static void logMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QMutexLocker locker(&s_logMutex);

    QString levelStr;
    switch (type) {
    case QtDebugMsg:    levelStr = "DEBUG"; break;
    case QtInfoMsg:     levelStr = "INFO"; break;
    case QtWarningMsg:  levelStr = "WARNING"; break;
    case QtCriticalMsg: levelStr = "ERROR"; break;
    case QtFatalMsg:    levelStr = "FATAL"; break;
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    QString category = context.category ? QString::fromUtf8(context.category) : "default";
    QString formatted = QString("[%1] [%2] [%3] %4\n")
                            .arg(timestamp, levelStr, category, msg);

    // Write to stderr (default Qt behavior)
    fprintf(stderr, "%s", qPrintable(formatted));

    // Write to log file
    if (!s_logFilePath.isEmpty()) {
        QFileInfo fi(s_logFilePath);

        // Rotate if file exceeds max size
        QFileInfo curFi(s_logFilePath);
        if (curFi.exists() && curFi.size() > s_maxFileSize) {
            locker.unlock(); // need to unlock before calling rotate which acquires lock
            rotateLogFile();
            locker.relock();
        }

        QDir().mkpath(fi.absolutePath());
        QFile file(s_logFilePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << formatted;
            file.close();
        }
    }

    if (type == QtFatalMsg) {
        abort();
    }
}

void Logger::init(const QString &logFilePath)
{
    s_logFilePath = logFilePath;
    s_oldHandler = qInstallMessageHandler(logMessageHandler);
}
