#include "data_restriction.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// 南美白对虾的一般警告范围
DataRestriction penaeusVannameiNormalData = NULL;
// 南美白对虾的严重警告范围
DataRestriction penaeusVannameiSeriousData = NULL;
// 南美白对虾的数据校验范围
DataRestriction penaeusVannameiValidData = NULL;
// 大口黑鲈的一般警告范围
DataRestriction micropterusSalmoidesNormalData = NULL;
// 大口黑鲈的严重警告范围
DataRestriction micropterusSalmoidesSeriousData = NULL;
// 大口黑鲈的数据校验范围
DataRestriction micropterusSalmoidesValidData = NULL;
// 南奥牡蛎的一般警告范围
DataRestriction crassostreaGigasNormalData = NULL;
// 南奥牡蛎的严重警告范围
DataRestriction crassostreaGigasSeriousData = NULL;
// 南奥牡蛎的数据校验范围
DataRestriction crassostreaGigasValidData = NULL;
// 保留 n 位小数的通用公式
double round_to_n_decimals(double value, int n) {
    double factor = pow(10, n);
    return round(value * factor) / factor;
}
// 初始化读取这些校验数据
void initDataRestriction(char* file) {
    penaeusVannameiNormalData = (DataRestriction)malloc(sizeof(struct DataRestriction));
    penaeusVannameiSeriousData = (DataRestriction)malloc(sizeof(struct DataRestriction));
    penaeusVannameiValidData = (DataRestriction)malloc(sizeof(struct DataRestriction));
    micropterusSalmoidesNormalData = (DataRestriction)malloc(sizeof(struct DataRestriction));
    micropterusSalmoidesSeriousData = (DataRestriction)malloc(sizeof(struct DataRestriction));
    micropterusSalmoidesValidData = (DataRestriction)malloc(sizeof(struct DataRestriction));
    crassostreaGigasNormalData = (DataRestriction)malloc(sizeof(struct DataRestriction));
    crassostreaGigasSeriousData = (DataRestriction)malloc(sizeof(struct DataRestriction));
    crassostreaGigasValidData = (DataRestriction)malloc(sizeof(struct DataRestriction));
    
    FILE* f = fopen(file, "r");

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
// 用于校验南美白对虾的数据
enum RestrictionType checkPenaeusVannameiData(struct WaterQuality * data) {
    return checkData(data, penaeusVannameiNormalData,
        penaeusVannameiSeriousData, penaeusVannameiValidData);
}
// 用于校验大口黑鲈的数据
enum RestrictionType checkMicropterusSalmoidesData(struct WaterQuality * data) {
    return checkData(data, micropterusSalmoidesNormalData
        , micropterusSalmoidesSeriousData, micropterusSalmoidesValidData);
}
// 用于校验南奥牡蛎的数据
enum RestrictionType checkCrassostreaGigasData(struct WaterQuality * data) {
    return checkData(data, crassostreaGigasNormalData
        , crassostreaGigasSeriousData, crassostreaGigasValidData);
}
// 通用数据校验方法
static enum RestrictionType checkData(struct WaterQuality * data, DataRestriction normal
    , DataRestriction serious, DataRestriction valid) {
    if (data->tmp >= normal->minTmp && data->tmp <= normal->maxTmp
        && data->doxygen >= normal->minDoxygen && data->doxygen <= normal->maxDoxygen
        && data->ph >= normal->minPh && data->ph <= normal->maxPh
        && data->ammonia >= normal->minAmmonia && data->ammonia <= normal->maxAmmonia) {
        return NORMAL;
    }
    if (data->tmp >= serious->minTmp && data->tmp <= serious->maxTmp
        && data->doxygen >= serious->minDoxygen && data->doxygen <= serious->maxDoxygen
        && data->ph >= serious->minPh && data->ph <= serious->maxPh
        && data->ammonia >= serious->minAmmonia && data->ammonia <= serious->maxAmmonia) {
        return NORMAL_ALERT;
    }
    if (data->tmp >= valid->minTmp && data->tmp <= valid->maxTmp
        && data->doxygen >= valid->minDoxygen && data->doxygen <= valid->maxDoxygen
        && data->ph >= valid->minPh && data->ph <= valid->maxPh
        && data->ammonia >= valid->minAmmonia && data->ammonia <= valid->maxAmmonia) {
        return SERIOUS_ALERT;
    }
    return INVALID_DATA;
}
// 数据校验回调方法，当某个字段不符合检验要求时，会回调传入的方法
// 回调包括字段名(即WaterQuality里的字段名), 是超过（1）范围还是低于范围（-1）,包括该字段的值
// 由于读取数据的进度问题
void checkDataCallbackFiled(struct WaterQuality * data, DataRestriction restriction
    , enum RestrictionType restrictType, void callback(enum WaterQualityEnum, enum RestrictionType, double value)) {
    if (data->tmp < restriction->minTmp || data->tmp > restriction->maxTmp) {
        callback(TMP, restrictType, data->tmp);
    }
    if (data->doxygen < restriction->minDoxygen || data->doxygen > restriction->maxDoxygen) {
        callback(DOXYGEN, restrictType, data->doxygen);
    }
    if (data->ph < restriction->minPh || data->ph > restriction->maxPh) {
        callback(PH, restrictType, data->ph);
    }
    if (data->ammonia < restriction->minAmmonia || data->ammonia > restriction->maxAmmonia) {
        callback(AMMONIA, restrictType, data->ammonia);
    }
}