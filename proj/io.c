#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "data.h"
#include "data_restriction.h"
#include "console.h"
#include <math.h>
const char* IO_LINE_WRITE_FMT = "%d, %.2f, %.2f, %.2f, %.2f, %s";
const char* IO_LINE_READ_FMT = "%d, %lf, %lf, %lf, %lf, %s %s";
const char* RECORD_FILE_NAME = "records.txt";
const char* RESTRICTION_FILE_NAME = "species.txt";

// 写入到文件，如果文件存在则会清空内容写入
void writeWaterQualityRecords(ArrayList list) {
    FILE* f = fopen(RECORD_FILE_NAME, "w");
    for (int i = 0; i < list->size; i++) {
        struct WaterQuality * quality = getAList(list, i);
        fprintf(f, IO_LINE_WRITE_FMT, quality->id, quality->tmp, quality->doxygen, quality->ph, quality->ammonia, quality->time);
        fprintf(f, "\n");
    }
    fflush(f);
    fclose(f);
}
// 读取文件，构建为ArrayList
ArrayList readWaterQualityRecords() {
    FILE* f = fopen(RECORD_FILE_NAME, "r");
    ArrayList list = createAListDefault();
    while (true) {
        struct WaterQuality * quality = malloc(sizeof(struct WaterQuality));
        char timeRight[11];
        int len = fscanf(f, IO_LINE_READ_FMT,
                 &(quality->id), &(quality->tmp), &(quality->doxygen)
                 , (&quality->ph), &(quality->ammonia), quality->time, timeRight);
        if (len != 7) {
            free(quality);
            fclose(f);
            break;
        }
        strcat(quality->time, " ");
        strcat(quality->time, timeRight);
        // 最后有一个换行符要去掉
        fgetc(f);
        addAList(list, quality);
    }
    fclose(f);
    return list;
}

void readRestrictionData() {
    penaeusVannameiNormalData = (DataRestriction)malloc(sizeof(struct DataRestriction));
    penaeusVannameiSeriousData = (DataRestriction)malloc(sizeof(struct DataRestriction));
    penaeusVannameiValidData = (DataRestriction)malloc(sizeof(struct DataRestriction));
    micropterusSalmoidesNormalData = (DataRestriction)malloc(sizeof(struct DataRestriction));
    micropterusSalmoidesSeriousData = (DataRestriction)malloc(sizeof(struct DataRestriction));
    micropterusSalmoidesValidData = (DataRestriction)malloc(sizeof(struct DataRestriction));
    crassostreaGigasNormalData = (DataRestriction)malloc(sizeof(struct DataRestriction));
    crassostreaGigasSeriousData = (DataRestriction)malloc(sizeof(struct DataRestriction));
    crassostreaGigasValidData = (DataRestriction)malloc(sizeof(struct DataRestriction));

    FILE* f = fopen(RESTRICTION_FILE_NAME, "r");

    char name[50], type[20], buf[256], line[256];
    double values[8];
    DataRestriction target = NULL;
    while (fgets(buf, sizeof(buf), f) != NULL) {
        // 移除行尾的换行符
        size_t len = strlen(buf);
        while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r')) {
            buf[len - 1] = '\0';
            len--;
        }
        // 跳过空行和注释行
        if (buf[0] == '\0' || (buf[0] == '/' && buf[1] == '/')) {
            continue;
        }
        if (sscanf(buf, "%[^,], %[^,], %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf",
                   name, type, &values[0], &values[1], &values[2], &values[3],
                   &values[4], &values[5], &values[6], &values[7]) == 10) {
            // 确定目标结构体
            if (strcmp(name, "penaeusVannamei") == 0) {
                if (strcmp(type, "normal") == 0) target = penaeusVannameiNormalData;
                else if (strcmp(type, "alert") == 0) target = penaeusVannameiSeriousData;
                else if (strcmp(type, "valid") == 0) target = penaeusVannameiValidData;
            } else if (strcmp(name, "micropterusSalmoides") == 0) {
                if (strcmp(type, "normal") == 0) target = micropterusSalmoidesNormalData;
                else if (strcmp(type, "alert") == 0) target = micropterusSalmoidesSeriousData;
                else if (strcmp(type, "valid") == 0) target = micropterusSalmoidesValidData;
            } else if (strcmp(name, "crassostreaGigas") == 0) {
                if (strcmp(type, "normal") == 0) target = crassostreaGigasNormalData;
                else if (strcmp(type, "alert") == 0) target = crassostreaGigasSeriousData;
                else if (strcmp(type, "valid") == 0) target = crassostreaGigasValidData;
            }
            if (target != NULL) {
                target->minTmp = ((double) ((int) round(values[0] * 100))) / 100.0f;
                target->maxTmp = ((double) ((int) round(values[1] * 100))) / 100.0f;
                target->minDoxygen = ((double) ((int) round(values[2] * 100))) / 100.0f;
                target->maxDoxygen = ((double) ((int) round(values[3] * 100))) / 100.0f;
                target->minPh = ((double) ((int) round(values[4] * 100))) / 100.0f;
                target->maxPh = ((double) ((int) round(values[5] * 100))) / 100.0f;
                target->minAmmonia = ((double) ((int) round(values[6] * 100))) / 100.0f;
                target->maxAmmonia = ((double) ((int) round(values[7] * 100))) / 100.0f;
                target = NULL;
            }
        }
        if (feof(f)) break;
    }

    fclose(f);
}

// 辅助方法：生成范围内的随机浮点数
double randomInRange(double min, double max) {
    return min + (double) rand() / RAND_MAX * (max - min);
}
bool randomBool(double probability) {
    return probability > randomInRange(0.0, 1.0);
}
double randomInRangeContinuous(double baseValue, double maxChange) {
    return randomInRange(baseValue - maxChange, baseValue + maxChange);
}
// 辅助方法：生成指定类型的时间字符串
void generateTimeStr(char* timeBuf, time_t baseTime, int index) {
    time_t recordTime = baseTime - (30 * (index)); // 每条记录间隔 30 秒
    struct tm* tm_info = localtime(&recordTime);
    strftime(timeBuf, 20, "%Y-%m-%d %H:%M:%S", tm_info);
}

// 用于生成随机数据
struct ArrayList* generateRandomWaterQualityData(int count, enum Mode mode) {
    time_t now = time(NULL);

    ArrayList list = createAList(count);

    DataRestriction normal = NULL;
    DataRestriction normalAlert = NULL;
    DataRestriction seriousAlert = NULL;
    if (mode == PENAEUS_VANNAMEI) {
        normal = penaeusVannameiNormalData;
        normalAlert = penaeusVannameiSeriousData;
        seriousAlert = penaeusVannameiValidData;
    } else if (mode == MICROPTERUS_SALMOIDES) {
        normal = micropterusSalmoidesNormalData;
        normalAlert = micropterusSalmoidesSeriousData;
        seriousAlert = micropterusSalmoidesValidData;
    } else if (mode == CRASSOSTRA_GIGAS) {
        normal = crassostreaGigasNormalData;
        normalAlert = crassostreaGigasSeriousData;
        seriousAlert = crassostreaGigasValidData;
    }
    // 下面常量规定了随机生成的方案
    // 每次生成的数据基于上一次的变化范围
    const double tmpChange = 0.5, doxygenChange = 0.1, phChange = 0.07, ammoniaChange = 0.015;
    // 当随机生成的数据达到正常范围的阈值时，有多大的概率随机到一般警告范围
    const double tmpNormalAlertProbability = 0.5, doxygenNormalAlertProbability = 0.5, phNormalAlertProbability = 0.5, ammoniaNormalAlertProbability = 0.5;
    // 当随机生成的数据达到一般警告范围的阈值时，有多大的概率随机到严重警告范围
    const double tmpSeriousAlertProbability = 0.5, doxygenSeriousAlertProbability = 0.5, phSeriousAlertProbability = 0.5, ammoniaSeriousAlertProbability = 0.5;
    // 修正系数，当生成的数据达到了严重警告范围的数据，下一次生成的数据有多大概率往正常的方向移动
    const double alphaTmp = 0.6, alphaDoxygen = 0.6, alphaPh = 0.6, alphaAmmonia = 0.6;
    // 记录的数据间隔(秒)
    const int second = 60 * 2;
    time_t randomTimeStart = now - (second * count);
    // 生成的数据是不可能超出合法范围的
    // 第一条数据从正常数据随机出来，后续的数据都基于上一条随机
    struct WaterQuality* first = (struct WaterQuality*) malloc(sizeof(struct WaterQuality));
    first->id = 1;
    first->tmp = randomInRange(normal->minTmp, normal->maxTmp);
    first->doxygen = randomInRange(normal->minDoxygen, normal->maxDoxygen);
    first->ammonia = randomInRange(normal->minAmmonia, normal->maxAmmonia);
    first->ph = randomInRange(normal->minPh, normal->maxPh);

    // 由于精度问题，可能会随机到负数，这里要做反转
    if (first->tmp < 0) first->tmp = -first->tmp;
    if (first->doxygen < 0) first->doxygen = -first->doxygen;
    if (first->ammonia < 0) first->ammonia = -first->ammonia;
    if (first->ph < 0) first->ph = -first->ph;

    // 生成时间字符串
    char timeBuf[20];
    generateTimeStr(timeBuf, randomTimeStart, count - 1);
    strcpy(first->time, timeBuf);
    randomTimeStart += second;

    addAList(list, first);


    for (int i = 1; i < count; i++) {
        struct WaterQuality* quality = (struct WaterQuality*) malloc(sizeof(struct WaterQuality));
        struct WaterQuality* lastQuality = (struct WaterQuality*) getAList(list, i - 1);
        quality->id = i + 1;

        // 在合法范围内生成随机数据
        // 温度
        {
            double delta = randomInRange(-tmpChange, tmpChange);
            double lastTmp = lastQuality->tmp;
            if (checkFelid(lastQuality, TMP, normal) == 0) {
                // 上一个是正常数据
                if (checkFelidValue(lastTmp + delta, TMP, normal) == 0) {
                    // 说明这次生成的还是正常数据，否则是一般警告
                    quality->tmp = lastTmp + delta;
                } else {
                    if (randomBool(tmpNormalAlertProbability)) {
                        quality->tmp = lastTmp + delta;
                    } else {
                        quality->tmp = lastTmp - delta;
                    }
                }
            } else if (checkFelid(lastQuality, TMP, normalAlert) == 0) {
                // 上一个是一般警告
                if (checkFelidValue(lastTmp + delta, TMP, normalAlert) == 0) {
                    // 说明这次生成的还是一般警告数据，否则是严重警告
                    quality->tmp = lastTmp + delta;
                } else {
                    if (randomBool(tmpSeriousAlertProbability)) {
                        quality->tmp = lastTmp + delta;
                    } else {
                        quality->tmp = lastTmp - delta;
                    }
                }
            } else if (checkFelid(lastQuality, TMP, seriousAlert) == 0) {
                // 上一个是严重警告数据
                if (checkFelidValue(lastTmp + delta, TMP, seriousAlert) == 0) {
                    // 说明这次生成的还是在严重警告之内，否则数据非法
                    if (abs(checkFelidValue(lastTmp + delta, TMP, normal)) > abs(checkFelidValue(lastTmp - delta, TMP, normal))) {
                        // 说明前者离正常数据远
                        quality->tmp = randomBool(alphaTmp) ? lastTmp - delta : lastTmp + delta;
                    } else {
                        quality->tmp = randomBool(alphaTmp) ? lastTmp + delta : lastTmp - delta;
                    }
                } else {
                    quality->tmp = lastTmp - delta;
                }
            }
        }
        // 含氧量
        {
            double delta = randomInRange(-doxygenChange, doxygenChange);
            double lastDoxygen = lastQuality->doxygen;
            if (checkFelid(lastQuality, DOXYGEN, normal) == 0) {
                // 上一个是正常数据
                if (checkFelidValue(lastDoxygen + delta, DOXYGEN, normal) == 0) {
                    // 说明这次生成的还是正常数据，否则是一般警告
                    quality->doxygen = lastDoxygen + delta;
                } else {
                    if (randomBool(doxygenNormalAlertProbability)) {
                        quality->doxygen = lastDoxygen + delta;
                    } else {
                        quality->doxygen = lastDoxygen - delta;
                    }
                }
            } else if (checkFelid(lastQuality, DOXYGEN, normalAlert) == 0) {
                // 上一个是一般警告
                if (checkFelidValue(lastDoxygen + delta, DOXYGEN, normalAlert) == 0) {
                    // 说明这次生成的还是一般警告数据，否则是严重警告
                    quality->doxygen = lastDoxygen + delta;
                } else {
                    if (randomBool(doxygenSeriousAlertProbability)) {
                        quality->doxygen = lastDoxygen + delta;
                    } else {
                        quality->doxygen = lastDoxygen - delta;
                    }
                }
            } else if (checkFelid(lastQuality, DOXYGEN, seriousAlert) == 0) {
                // 上一个是严重警告数据
                if (checkFelidValue(lastDoxygen + delta, DOXYGEN, seriousAlert) == 0) {
                    // 说明这次生成的还是在严重警告之内，否则数据非法
                    if (abs(checkFelidValue(lastDoxygen + delta, DOXYGEN, normal)) > abs(checkFelidValue(lastDoxygen - delta, DOXYGEN, normal))) {
                        // 说明前者离正常数据远
                        quality->doxygen = randomBool(alphaDoxygen) ? lastDoxygen - delta : lastDoxygen + delta;
                    } else {
                        quality->doxygen = randomBool(alphaDoxygen) ? lastDoxygen + delta : lastDoxygen - delta;
                    }
                } else {
                    quality->doxygen = lastDoxygen - delta;
                }
            }
        }
        // 氨氮
        {
            double delta = randomInRange(-ammoniaChange, ammoniaChange);
            double lastAmmonia = lastQuality->ammonia;
            if (checkFelid(lastQuality, AMMONIA, normal) == 0) {
                // 上一个是正常数据
                if (checkFelidValue(lastAmmonia + delta, AMMONIA, normal) == 0) {
                    // 说明这次生成的还是正常数据，否则是一般警告
                    quality->ammonia = lastAmmonia + delta;
                } else {
                    if (randomBool(ammoniaNormalAlertProbability)) {
                        quality->ammonia = lastAmmonia + delta;
                    } else {
                        quality->ammonia = lastAmmonia - delta;
                    }
                }
            } else if (checkFelid(lastQuality, AMMONIA, normalAlert) == 0) {
                // 上一个是一般警告
                if (checkFelidValue(lastAmmonia + delta, AMMONIA, normalAlert) == 0) {
                    // 说明这次生成的还是一般警告数据，否则是严重警告
                    quality->ammonia = lastAmmonia + delta;
                } else {
                    if (randomBool(ammoniaSeriousAlertProbability)) {
                        quality->ammonia = lastAmmonia + delta;
                    } else {
                        quality->ammonia = lastAmmonia - delta;
                    }
                }
            } else if (checkFelid(lastQuality, AMMONIA, seriousAlert) == 0) {
                // 上一个是严重警告数据
                if (checkFelidValue(lastAmmonia + delta, AMMONIA, seriousAlert) == 0) {
                    // 说明这次生成的还是在严重警告之内，否则数据非法
                    if (abs(checkFelidValue(lastAmmonia + delta, AMMONIA, normal)) > abs(checkFelidValue(lastAmmonia - delta, AMMONIA, normal))) {
                        // 说明前者离正常数据远
                        quality->ammonia = randomBool(alphaAmmonia) ? lastAmmonia - delta : lastAmmonia + delta;
                    } else {
                        quality->ammonia = randomBool(alphaAmmonia) ? lastAmmonia + delta : lastAmmonia - delta;
                    }
                } else {
                    quality->ammonia = lastAmmonia - delta;
                }
            }
        }
        // PH
        {
            double delta = randomInRange(-phChange, phChange);
            double lastPh = lastQuality->ph;
            if (checkFelid(lastQuality, PH, normal) == 0) {
                // 上一个是正常数据
                if (checkFelidValue(lastPh + delta, PH, normal) == 0) {
                    // 说明这次生成的还是正常数据，否则是一般警告
                    quality->ph = lastPh + delta;
                } else {
                    if (randomBool(phNormalAlertProbability)) {
                        quality->ph = lastPh + delta;
                    } else {
                        quality->ph = lastPh - delta;
                    }
                }
            } else if (checkFelid(lastQuality, PH, normalAlert) == 0) {
                // 上一个是一般警告
                if (checkFelidValue(lastPh + delta, PH, normalAlert) == 0) {
                    // 说明这次生成的还是一般警告数据，否则是严重警告
                    quality->ph = lastPh + delta;
                } else {
                    if (randomBool(phSeriousAlertProbability)) {
                        quality->ph = lastPh + delta;
                    } else {
                        quality->ph = lastPh - delta;
                    }
                }
            } else if (checkFelid(lastQuality, PH, seriousAlert) == 0) {
                // 上一个是严重警告数据
                if (checkFelidValue(lastPh + delta, PH, seriousAlert) == 0) {
                    // 说明这次生成的还是在严重警告之内，否则数据非法
                    if (abs(checkFelidValue(lastPh + delta, PH, normal)) > abs(checkFelidValue(lastPh - delta, PH, normal))) {
                        // 说明前者离正常数据远
                        quality->ph = randomBool(alphaPh) ? lastPh - delta : lastPh + delta;
                    } else {
                        quality->ph = randomBool(alphaPh) ? lastPh + delta : lastPh - delta;
                    }
                } else {
                    quality->ph = lastPh - delta;
                }
            }
        }
        // 由于精度问题，可能会随机到负数，这里要做反转
        if (quality->tmp < 0) quality->tmp = -quality->tmp;
        if (quality->doxygen < 0) quality->doxygen = -quality->doxygen;
        if (quality->ammonia < 0) quality->ammonia = -quality->ammonia;
        if (quality->ph < 0) quality->ph = -quality->ph;

        // 生成时间字符串
        generateTimeStr(timeBuf, randomTimeStart, count - 1 - i);
        strcpy(quality->time, timeBuf);
        randomTimeStart += second;

        addAList(list, quality);
    }

    return list;
}