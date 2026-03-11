#include "data_restriction.h"
#include <stdlib.h>
// 南美白对虾的一般警告范围
DataRestrictionPointer penaeusVannameiNormalData = NULL;
// 南美白对虾的严重警告范围
DataRestrictionPointer penaeusVannameiSeriousData = NULL;
// 南美白对虾的数据校验范围
DataRestrictionPointer penaeusVannameiValidData = NULL;
// 大口黑鲈的一般警告范围
DataRestrictionPointer micropterusSalmoidesNormalData = NULL;
// 大口黑鲈的严重警告范围
DataRestrictionPointer micropterusSalmoidesSeriousData = NULL;
// 大口黑鲈的数据校验范围
DataRestrictionPointer micropterusSalmoidesValidData = NULL;
// 南奥牡蛎的一般警告范围
DataRestrictionPointer crassostreaGigasNormalData = NULL;
// 南奥牡蛎的严重警告范围
DataRestrictionPointer crassostreaGigasSeriousData = NULL;
// 南奥牡蛎的数据校验范围
DataRestrictionPointer crassostreaGigasValidData = NULL;

// 初始化这些校验数据
void initDataRestriction() {


}
// 用于校验南美白对虾的数据
enum RestrictionType checkPenaeusVannameiData(WaterQuality data) {
    return checkData(data, penaeusVannameiNormalData,
        penaeusVannameiSeriousData, penaeusVannameiValidData);
}
// 用于校验大口黑鲈的数据
enum RestrictionType checkMicropterusSalmoidesData(WaterQuality data) {
    return checkData(data, micropterusSalmoidesNormalData
        , micropterusSalmoidesSeriousData, micropterusSalmoidesValidData);
}
// 用于校验南奥牡蛎的数据
enum RestrictionType checkCrassostreaGigasData(WaterQuality data) {
    return checkData(data, crassostreaGigasNormalData
        , crassostreaGigasSeriousData, crassostreaGigasValidData);
}
// 通用数据校验方法
static enum RestrictionType checkData(WaterQuality data, DataRestrictionPointer normal
    , DataRestrictionPointer serious, DataRestrictionPointer valid) {
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
void checkDataCallbackFiled(WaterQuality data, DataRestrictionPointer restriction
    , enum RestrictionType restrictType, void callback(enum WaterQualityEnum, enum RestrictionType, float value)) {
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