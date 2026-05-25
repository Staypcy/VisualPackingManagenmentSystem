#ifndef FEERULE_H
#define FEERULE_H

struct FeeRule
{
    double firstHourRate;       // 首小时费率
    double additionalHourRate;  // 额外小时费率
    double dailyMax;            // 每日最高收费
    int freeMinutes;            // 免费时长（分钟）

    FeeRule() : firstHourRate(5.0), additionalHourRate(3.0), dailyMax(50.0), freeMinutes(15) {}
};

#endif // FEERULE_H
