#include "feeservice.h"
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QDebug>

FeeService* FeeService::instance = nullptr;

FeeService::FeeService(QObject *parent) : QObject(parent)
{
    loadRule();
}

FeeService::~FeeService()
{
    saveRule();
}

FeeService* FeeService::getInstance()
{
    if (!instance) {
        instance = new FeeService();
    }
    return instance;
}

void FeeService::setRule(const FeeRule& rule)
{
    currentRule = rule;
    saveRule();
}

FeeRule FeeService::getRule() const
{
    return currentRule;
}

double FeeService::calculateFee(const QDateTime& enterTime, const QDateTime& exitTime) const
{
    qint64 seconds = enterTime.secsTo(exitTime);
    
    if (seconds < 0) {
        return 0.0;
    }
    
    // 检查是否在免费时长内
    if (seconds <= currentRule.freeMinutes * 60) {
        return 0.0;
    }
    
    // 计算总分钟数（扣除免费时长）
    double totalMinutes = (seconds - currentRule.freeMinutes * 60) / 60.0;
    
    // 计算小时数（向上取整）
    int hours = static_cast<int>(ceil(totalMinutes / 60.0));
    
    double fee = 0.0;
    
    if (hours <= 1) {
        fee = currentRule.firstHourRate;
    } else {
        fee = currentRule.firstHourRate + (hours - 1) * currentRule.additionalHourRate;
    }
    
    // 应用每日最高收费限制
    fee = qMin(fee, currentRule.dailyMax);
    
    return fee;
}

void FeeService::loadRule()
{
    QString filePath = QCoreApplication::applicationDirPath() + "/fee_rule.txt";
    QFile file(filePath);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "使用默认计费规则";
        return;
    }
    
    QTextStream in(&file);
    QString line;
    
    while (!in.atEnd()) {
        line = in.readLine();
        QStringList parts = line.split("=");
        if (parts.size() == 2) {
            QString key = parts[0].trimmed();
            double value = parts[1].trimmed().toDouble();
            
            if (key == "firstHourRate") {
                currentRule.firstHourRate = value;
            } else if (key == "additionalHourRate") {
                currentRule.additionalHourRate = value;
            } else if (key == "dailyMax") {
                currentRule.dailyMax = value;
            } else if (key == "freeMinutes") {
                currentRule.freeMinutes = static_cast<int>(value);
            }
        }
    }
    
    file.close();
}

void FeeService::saveRule()
{
    QString filePath = QCoreApplication::applicationDirPath() + "/fee_rule.txt";
    QFile file(filePath);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "无法保存计费规则";
        return;
    }
    
    QTextStream out(&file);
    out << "firstHourRate=" << currentRule.firstHourRate << "\n";
    out << "additionalHourRate=" << currentRule.additionalHourRate << "\n";
    out << "dailyMax=" << currentRule.dailyMax << "\n";
    out << "freeMinutes=" << currentRule.freeMinutes << "\n";
    
    file.close();
}