#ifndef FEESERVICE_H
#define FEESERVICE_H

#include <QObject>
#include <QDateTime>

class FeeRule {
public:
    double firstHourRate;      //首小时免费率
    double additionalHourRate; // 额外小时费率
    double dailyMax;           // 每日最高收费
    int freeMinutes;           // 免费时长（分钟）
    
    FeeRule() : firstHourRate(5.0), additionalHourRate(3.0), dailyMax(50.0), freeMinutes(15) {}
};

class FeeService : public QObject
{
    Q_OBJECT
public:
    static FeeService* getInstance();
    
    void setRule(const FeeRule& rule);
    FeeRule getRule() const;
    
    double calculateFee(const QDateTime& enterTime, const QDateTime& exitTime) const;
    
    void loadRule();
    void saveRule();
    
private:
    FeeService(QObject *parent = nullptr);
    ~FeeService();
    
    static FeeService* instance;
    FeeRule currentRule;
};

#endif // FEESERVICE_H