#ifndef IFEESERVICE_H
#define IFEESERVICE_H

#include <QDateTime>
#include "entities/feerule.h"

class IFeeService
{
public:
    virtual ~IFeeService() = default;

    virtual void setRule(const FeeRule &rule) = 0;
    virtual FeeRule getRule() const = 0;
    virtual double calculateFee(const QDateTime &enterTime, const QDateTime &exitTime) const = 0;
    virtual bool loadRule() = 0;
    virtual bool saveRule() = 0;
};

#endif // IFEESERVICE_H
