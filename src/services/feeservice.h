#ifndef FEESERVICE_H
#define FEESERVICE_H

#include <QObject>
#include <QDateTime>
#include "entities/feerule.h"
#include "services/ifeeservice.h"

class DatabaseManager;

class FeeService : public QObject, public IFeeService
{
    Q_OBJECT
public:
    explicit FeeService(DatabaseManager *dbManager, QObject *parent = nullptr);
    ~FeeService() override;

    void setRule(const FeeRule &rule) override;
    FeeRule getRule() const override;
    double calculateFee(const QDateTime &enterTime, const QDateTime &exitTime) const override;
    bool loadRule() override;
    bool saveRule() override;

private:
    DatabaseManager *m_dbManager;
    FeeRule currentRule;
};

#endif // FEESERVICE_H
