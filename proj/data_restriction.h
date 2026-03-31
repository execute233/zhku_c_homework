// 记录限制超过警报的数据结构，同时对数据合法性进行校验
#ifndef DATA_RESTRICTION_H
#define DATA_RESTRICTION_H

#include "data.h"
// 警告范围(分为一般警告与严重警告,以及数据校验)
// 当数据在一般警告范围内时，处于正常状态
// 当数据在严重警告范围内时，但不在一般警告范围内，处于一般警告状态
// 当数据超出严重警告范围时，但数据在合法范围内，处于严重警告状态
// 当数据超出了其校验范围时，该数据应当丢弃，因为数据不合法
struct DataRestriction {
    double minTmp; // 最小温度
    double maxTmp; // 最大温度
    double minDoxygen; // 最小悬浮氧
    double maxDoxygen; // 最大悬浮氧
    double minPh; // 最小ph
    double maxPh; // 最大ph
    double minAmmonia; // 最小氨氮
    double maxAmmonia; // 最大氨氮
};
// 用于指示数据的警告范围、或者数据是否合法
enum RestrictionType {
    NORMAL, // 正常
    NORMAL_ALERT, // 一般警告
    SERIOUS_ALERT, // 严重警告
    INVALID_DATA // 非法数据
};
/**
 * 下面都是一些数据范围，是
 * 非法数据 < ValidData < 严重警告 < SeriousData < 一般警告 < NormalData < 正常
 * < NormalData < 一般警告 < SeriousData < 严重警告 < ValidaData < 非法数据
 ***/
// 南美白对虾的一般警告范围
extern struct DataRestriction* penaeusVannameiNormalData;
// 南美白对虾的严重警告范围
extern struct DataRestriction* penaeusVannameiSeriousData;
// 南美白对虾的数据校验范围
extern struct DataRestriction* penaeusVannameiValidData;
// 大口黑鲈的一般警告范围
extern struct DataRestriction* micropterusSalmoidesNormalData;
// 大口黑鲈的严重警告范围
extern struct DataRestriction* micropterusSalmoidesSeriousData;
// 大口黑鲈的数据校验范围
extern struct DataRestriction* micropterusSalmoidesValidData;
// 南奥牡蛎的一般警告范围
extern struct DataRestriction* crassostreaGigasNormalData;
// 南奥牡蛎的严重警告范围
extern struct DataRestriction* crassostreaGigasSeriousData;
// 南奥牡蛎的数据校验范围
extern struct DataRestriction* crassostreaGigasValidData;
// 用于校验南美白对虾的数据
enum RestrictionType checkPenaeusVannameiData(struct WaterQuality * data);
// 用于校验大口黑鲈的数据
enum RestrictionType checkMicropterusSalmoidesData(struct WaterQuality * data);
// 用于校验南奥牡蛎的数据
enum RestrictionType checkCrassostreaGigasData(struct WaterQuality * data);
// 通用数据校验方法
static enum RestrictionType checkData(struct WaterQuality * data, struct DataRestriction* normal
    , struct DataRestriction* serious, struct DataRestriction* valid);
// 校验字段，符合标准时返回0，小于标准时<0，大于标准时返回>0
int checkFelid(struct WaterQuality* quality, enum WaterQualityEnum field, struct DataRestriction* restriction);
int checkFelidValue(double value, enum WaterQualityEnum field, struct DataRestriction* restriction);

#endif