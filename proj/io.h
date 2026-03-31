// 用于操作文件读写.应该是
#ifndef IO_H
#define IO_H
#include "../lib/array_list.h"
#include "console.h"
// 写入到水质记录文件，如果文件存在则会清空内容写入
void writeWaterQualityRecords(struct ArrayList* list);
// 读取水质记录文件，构建为ArrayList
struct ArrayList* readWaterQualityRecords();
// 读取限制的数据
void readRestrictionData();
// 随机生成水质数据
struct ArrayList* generateRandomWaterQualityData(int count, enum Mode mode);
#endif