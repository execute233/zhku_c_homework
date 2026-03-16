// 用于操作文件读写.应该是
#ifndef IO_H
#define IO_H
#include "../lib/array_list.h"
// 写入到文件，如果文件存在则会清空内容写入
void writeWaterQualityRecords(ArrayList list);
// 读取文件，构建为ArrayList
ArrayList readWaterQualityRecords();
// 读取限制的数据
void readRestrictionData();
// 随机生成数据
struct ArrayList * generateAndSaveRandomWaterQualityData(int count, enum Mode mode);
#endif