
#include <stdio.h>

#include "statistics.h"
#include "console.h"
#include "data_restriction.h"
// 查看统计数据
void seeStatistics() {
    if (globalRecordList->size == 0) {
        printDefaultAutoEnter("无统计数据");
        Sleep(3000);
        return;
    }
    // 这里统计计算
    double size = globalRecordList->size;
    struct DataRestriction* normalRestriction;
    struct DataRestriction* seriousRestriction;
    struct DataRestriction* validRestriction;
    if (mode == PENAEUS_VANNAMEI) {
        normalRestriction = penaeusVannameiNormalData;
        seriousRestriction = penaeusVannameiSeriousData;
        validRestriction = penaeusVannameiValidData;
    } else if (mode == MICROPTERUS_SALMOIDES) {
        normalRestriction = micropterusSalmoidesNormalData;
        seriousRestriction = micropterusSalmoidesSeriousData;
        validRestriction = micropterusSalmoidesValidData;
    } else {
        normalRestriction = crassostreaGigasNormalData;
        seriousRestriction = crassostreaGigasSeriousData;
        validRestriction = crassostreaGigasValidData;
    }
    struct WaterQuality* q1 = getAList(globalRecordList, 0);
    double tmpSum = 0, doxygenSum = 0, phSum = 0, ammoniaSum = 0;
    double tmpMax = q1->tmp, doxygenMax = q1->doxygen, phMax = q1->ph, ammoniaMax = q1->ammonia;
    double tmpMin = q1->tmp, doxygenMin = q1->doxygen, phMin = q1->ph, ammoniaMin = q1->ammonia;
    int normalTmpCount = 0, normalDoxygenCount = 0, normalPhCount = 0, normalAmmoniaCount = 0;
    int normalAlertTmpCount = 0, normalAlertDoxygenCount = 0, normalAlertPhCount = 0, normalAlertAmmoniaCount = 0;
    int seriousAlertTmpCount = 0, seriousAlertDoxygenCount = 0, seriousAlertPhCount = 0, seriousAlertAmmoniaCount = 0;
    for (int i = 0; i < globalRecordList->size; i++) {
        struct WaterQuality* q = getAList(globalRecordList, i);
        tmpSum += q->tmp;
        doxygenSum += q->doxygen;
        phSum += q->ph;
        ammoniaSum += q->ammonia;
        tmpMax = q->tmp > tmpMax ? q->tmp : tmpMax;
        doxygenMax = q->doxygen > doxygenMax ? q->doxygen : doxygenMax;
        phMax = q->ph > phMax ? q->ph : phMax;
        ammoniaMax = q->ammonia > ammoniaMax ? q->ammonia : ammoniaMax;
        tmpMin = q->tmp < tmpMin ? q->tmp : tmpMin;
        doxygenMin = q->doxygen < doxygenMin ? q->doxygen : doxygenMin;
        phMin = q->ph < phMin ? q->ph : phMin;
        ammoniaMin = q->ammonia < ammoniaMin ? q->ammonia : ammoniaMin;
        if (q->tmp >= normalRestriction->minTmp && q->tmp <= normalRestriction->maxTmp) {
            normalTmpCount++;
        } else if (q->tmp >= seriousRestriction->minTmp && q->tmp <= seriousRestriction->maxTmp) {
            normalAlertTmpCount++;
        } else if (q->tmp >= validRestriction->minTmp && q->tmp <= validRestriction->maxTmp) {
            seriousAlertTmpCount++;
        }
        if (q->doxygen >= normalRestriction->minDoxygen && q->doxygen <= normalRestriction->maxDoxygen) {
            normalDoxygenCount++;
        } else if (q->doxygen >= seriousRestriction->minDoxygen && q->doxygen <= seriousRestriction->maxDoxygen) {
            normalAlertDoxygenCount++;
        } else if (q->doxygen >= validRestriction->minDoxygen && q->doxygen <= validRestriction->maxDoxygen) {
            seriousAlertDoxygenCount++;
        }
        if (q->ph >= normalRestriction->minPh && q->ph <= normalRestriction->maxPh) {
            normalPhCount++;
        } else if (q->ph >= seriousRestriction->minPh && q->ph <= seriousRestriction->maxPh) {
            normalAlertPhCount++;
        } else if (q->ph >= validRestriction->minPh && q->ph <= validRestriction->maxPh) {
            seriousAlertPhCount++;
        }
        if (q->ammonia >= normalRestriction->minAmmonia && q->ammonia <= normalRestriction->maxAmmonia) {
            normalAmmoniaCount++;
        } else if (q->ammonia >= seriousRestriction->minAmmonia && q->ammonia <= seriousRestriction->maxAmmonia) {
            normalAlertAmmoniaCount++;
        } else if (q->ammonia >= validRestriction->minAmmonia && q->ammonia <= validRestriction->maxAmmonia) {
            seriousAlertAmmoniaCount++;
        }
    }
    //        温度 溶解氧 PH 氨氮
    // 平均值   xx  xx   xx  xx
    // 最大值   xx  xx   xx  xx
    // 最小值   xx  xx   xx  xx
    // 正常(%)
    // 一般告警
    // 严重告警
    double tmpAvg = tmpSum / size, doxygenAvg = doxygenSum / size
        , phAvg = phSum / size, ammoniaAvg = ammoniaSum / size;
    printf("\t\t温度\t溶解氧\tPH\t氨氮\n");
    printf("平均值\t\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n", tmpAvg, doxygenAvg, phAvg, ammoniaAvg);
    printf("最大值\t\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n", tmpMax, doxygenMax, phMax, ammoniaMax);
    printf("最小值\t\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n", tmpMin, doxygenMin, phMin, ammoniaMin);
    printf("正常(%%)\t\t%.2lf%%\t%.2lf%%\t%.2lf%%\t%.2lf%%\n", 100.0 * normalTmpCount / size, 100.0 * normalDoxygenCount / size, 100.0 * normalPhCount / size, 100.0 * normalAmmoniaCount / size);
    printf("一般告警(%%)\t%.2lf%%\t%.2lf%%\t%.2lf%%\t%.2lf%%\n", 100.0 * normalAlertTmpCount / size, 100.0 * normalAlertDoxygenCount / size, 100.0 * normalAlertPhCount / size, 100.0 * normalAlertAmmoniaCount / size);
    printf("严重告警(%%)\t%.2lf%%\t%.2lf%%\t%.2lf%%\t%.2lf%%\n", 100.0 * seriousAlertTmpCount / size, 100.0 * seriousAlertDoxygenCount / size, 100.0 * seriousAlertPhCount / size, 100.0 * seriousAlertAmmoniaCount / size);
    printf("按下Enter键返回");
    waitForRightKey(ENTER);
    clearScreen();
}