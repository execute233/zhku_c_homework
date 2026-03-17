#include "data_restriction.h"
#include <stddef.h>
#include <math.h>
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
int checkFelid(struct WaterQuality* quality, enum WaterQualityEnum field, DataRestriction restriction) {
    if (field == TMP) {
        return checkFelidValue(quality->tmp, TMP, restriction);
    }
    if (field == DOXYGEN) {
        return checkFelidValue(quality->doxygen, DOXYGEN, restriction);
    }
    if (field == AMMONIA) {
        return checkFelidValue(quality->ammonia, AMMONIA, restriction);
    }
    if (field == PH) {
        return checkFelidValue(quality->ph, PH, restriction);
    }
    return 0;
}
int checkFelidValue(double value, enum WaterQualityEnum field, DataRestriction restriction) {
    int result = 0;
    if (field == TMP) {
        if (value > restriction->maxTmp) result = (int) (value - restriction->maxTmp) * 10000;
        if (value < restriction->minTmp) result = (int) (value - restriction->minTmp) * 10000;
    } else if (field == DOXYGEN) {
        if (value > restriction->maxDoxygen) result = (int) (value - restriction->maxDoxygen) * 10000;
        if (value < restriction->minDoxygen) result = (int) (value - restriction->minDoxygen) * 10000;
    } else if (field == AMMONIA) {
        if (value > restriction->maxAmmonia) result = (int) (value - restriction->maxAmmonia) * 10000;
        if (value < restriction->minAmmonia) result = (int) (value - restriction->minAmmonia) * 10000;
    } else if (field == PH) {
        if (value > restriction->maxPh) result = (int) (value - restriction->maxPh) * 10000;
        if (value < restriction->minPh) result = (int) (value - restriction->minPh) * 10000;
    }
    return result;
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