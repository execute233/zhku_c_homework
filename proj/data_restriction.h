// 记录限制超过警报的数据结构，同时对数据合法性进行校验
#ifndef DATA_RESTRICTION_H
#define DATA_RESTRICTION_H

#include "data.h"
// 警告范围(分为一般警告与严重警告,以及数据校验)
// 当数据在一般警告范围内时，处于正常状态
// 当数据在严重警告范围内时，但不在一般警告范围内，处于一般警告状态
// 当数据超出严重警告范围时，但数据在合法范围内，处于严重警告状态
// 当数据超出了其校验范围时，该数据应当丢弃，因为数据不合法
typedef struct DataRestriction {
    double minTmp;
    double maxTmp;
    double minDoxygen;
    double maxDoxygen;
    double minPh;
    double maxPh;
    double minAmmonia;
    double maxAmmonia;
} * DataRestriction;
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
extern DataRestriction penaeusVannameiNormalData;
// 南美白对虾的严重警告范围
extern DataRestriction penaeusVannameiSeriousData;
// 南美白对虾的数据校验范围
extern DataRestriction penaeusVannameiValidData;
// 大口黑鲈的一般警告范围
extern DataRestriction micropterusSalmoidesNormalData;
// 大口黑鲈的严重警告范围
extern DataRestriction micropterusSalmoidesSeriousData;
// 大口黑鲈的数据校验范围
extern DataRestriction micropterusSalmoidesValidData;
// 南奥牡蛎的一般警告范围
extern DataRestriction crassostreaGigasNormalData;
// 南奥牡蛎的严重警告范围
extern DataRestriction crassostreaGigasSeriousData;
// 南奥牡蛎的数据校验范围
extern DataRestriction crassostreaGigasValidData;
// 用于校验南美白对虾的数据
enum RestrictionType checkPenaeusVannameiData(struct WaterQuality * data);
// 用于校验大口黑鲈的数据
enum RestrictionType checkMicropterusSalmoidesData(struct WaterQuality * data);
// 用于校验南奥牡蛎的数据
enum RestrictionType checkCrassostreaGigasData(struct WaterQuality * data);
// 通用数据校验方法
static enum RestrictionType checkData(struct WaterQuality * data, DataRestriction normal
    , DataRestriction serious, DataRestriction valid);
// 校验字段，符合标准时返回0，小于标准时<0，大于标准时返回>0
int checkFelid(struct WaterQuality* quality, enum WaterQualityEnum field, DataRestriction restriction);
int checkFelidValue(double value, enum WaterQualityEnum field, DataRestriction restriction);

#endif