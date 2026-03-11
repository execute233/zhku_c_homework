//
#ifndef DATA_H
#define DATA_H
// 监测数据结构体
typedef struct WaterQuality {
   int id; // 记录唯一id
   float tmp; // 水温 度
   float doxygen; // 溶解氧 mg/L
   float ph; // PH值 pH
   float ammonia; // 氨氮含量 mg/L
   char time[20]; // 时间记录字符串，格式 yyyy-MM-dd HH:mm:ss
} * WaterQuality;
// 用于指示上面的一部分字段的
enum WaterQualityEnum {
   TMP,
   DOXYGEN,
   PH,
   AMMONIA,
};

#endif