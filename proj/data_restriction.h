// 记录限制超过警报的数据结构，同时对数据合法性进行校验
#ifndef DATA_RESTRICTION_H
#define DATA_RESTRICTION_H
#include <stddef.h>

#include "data.h"
// 警告范围(分为一般警告与严重警告,以及数据校验)
// 当数据在一般警告范围内时，处于正常状态
// 当数据在严重警告范围内时，但不在一般警告范围内，处于一般警告状态
// 当数据超出严重警告范围时，但数据在合法范围内，处于严重警告状态
// 当数据超出了其校验范围时，该数据应当丢弃，因为数据不合法
typedef struct DataRestriction {
    float minTmp;
    float maxTmp;
    float minDoxygen;
    float maxDoxygen;
    float minPh;
    float maxPh;
    float minAmmonia;
    float maxAmmonia;
} * DataRestrictionPointer;
// 用于指示数据的警告范围、或者数据是否合法
enum RestrictionType {
    NORMAL, // 正常
    NORMAL_ALERT, // 一般警告
    SERIOUS_ALERT, // 严重警告
    INVALID_DATA // 非法数据
};
// 南美白对虾的一般警告范围
extern DataRestrictionPointer penaeusVannameiNormalData;
// 南美白对虾的严重警告范围
extern DataRestrictionPointer penaeusVannameiSeriousData;
// 南美白对虾的数据校验范围
extern DataRestrictionPointer penaeusVannameiValidData;
// 大口黑鲈的一般警告范围
extern DataRestrictionPointer micropterusSalmoidesNormalData;
// 大口黑鲈的严重警告范围
extern DataRestrictionPointer micropterusSalmoidesSeriousData;
// 大口黑鲈的数据校验范围
extern DataRestrictionPointer micropterusSalmoidesValidData;
// 南奥牡蛎的一般警告范围
extern DataRestrictionPointer crassostreaGigasNormalData;
// 南奥牡蛎的严重警告范围
extern DataRestrictionPointer crassostreaGigasSeriousData;
// 南奥牡蛎的数据校验范围
extern DataRestrictionPointer crassostreaGigasValidData;
// 方法，用于初始化这些校验数据
void initDataRestriction();
// 用于校验南美白对虾的数据
enum RestrictionType checkPenaeusVannameiData(WaterQuality data);
// 用于校验大口黑鲈的数据
enum RestrictionType checkMicropterusSalmoidesData(WaterQuality data);
// 用于校验南奥牡蛎的数据
enum RestrictionType checkCrassostreaGigasData(WaterQuality data);
// 通用数据校验方法
static enum RestrictionType checkData(WaterQuality data, DataRestrictionPointer normal
    , DataRestrictionPointer serious, DataRestrictionPointer valid);
// 数据校验回调方法，当某个字段不符合检验要求时，会回调传入的方法
// 回调包括字段(即WaterQualityEnum), 是超过（1）范围还是低于范围（-1）,包括该字段的值
void checkDataCallbackFiled(WaterQuality data, DataRestrictionPointer restriction
    , enum RestrictionType, void callback(enum WaterQualityEnum, enum RestrictionType, float value));
#endif