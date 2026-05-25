#include "ui/mainwindow.h"
#include "utils/logger.h"
#include "database/databasemanager.h"
#include "services/feeservice.h"
#include "services/authservice.h"

#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    Logger::init(QCoreApplication::applicationDirPath() + "/vpms.log");

    QApplication a(argc, argv);

    DatabaseManager dbManager(QCoreApplication::applicationDirPath() + "/vpms.db");
    Result<void> dbResult = dbManager.initialize();
    if (dbResult.isError()) {
        LOG_ERROR() << "Database initialization failed:" << dbResult.errorMessage();
        return 1;
    }

    FeeService feeService(&dbManager);
    AuthService authService(&dbManager);

    MainWindow w(&feeService, &authService);
    w.show();
    return QCoreApplication::exec();
}
