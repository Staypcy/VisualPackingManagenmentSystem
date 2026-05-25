#include "services/feeservice.h"
#include "database/databasemanager.h"
#include "utils/logger.h"

FeeService::FeeService(DatabaseManager *dbManager, QObject *parent)
    : QObject(parent)
    , m_dbManager(dbManager)
{
    loadRule();
}

FeeService::~FeeService()
{
    saveRule();
}

void FeeService::setRule(const FeeRule &rule)
{
    currentRule = rule;
    saveRule();
}

FeeRule FeeService::getRule() const
{
    return currentRule;
}

double FeeService::calculateFee(const QDateTime &enterTime, const QDateTime &exitTime) const
{
    qint64 seconds = enterTime.secsTo(exitTime);

    if (seconds < 0) {
        return 0.0;
    }

    if (seconds <= currentRule.freeMinutes * 60) {
        return 0.0;
    }

    double totalMinutes = (seconds - currentRule.freeMinutes * 60) / 60.0;
    int hours = static_cast<int>(ceil(totalMinutes / 60.0));

    double fee = 0.0;
    if (hours <= 1) {
        fee = currentRule.firstHourRate;
    } else {
        fee = currentRule.firstHourRate + (hours - 1) * currentRule.additionalHourRate;
    }

    fee = qMin(fee, currentRule.dailyMax);
    return fee;
}

bool FeeService::loadRule()
{
    Result<FeeRule> result = m_dbManager->getFeeRule();
    if (result.isOk()) {
        currentRule = result.value();
        return true;
    }
    LOG_WARNING() << "Failed to load fee rule from database, using defaults:"
                  << result.errorMessage();
    return false;
}

bool FeeService::saveRule()
{
    Result<void> result = m_dbManager->saveFeeRule(currentRule);
    if (result.isError()) {
        LOG_ERROR() << "Failed to save fee rule:" << result.errorMessage();
        return false;
    }
    return true;
}
