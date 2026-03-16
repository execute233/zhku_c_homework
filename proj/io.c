#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "data_restriction.h"
#include "../proj/data.h"
#include <string.h>
const char* IO_LINE_WRITE_FMT = "%d, %.2f, %.2f, %.2f, %.2f, %s";
const char* IO_LINE_READ_FMT = "%d, %lf, %lf, %lf, %lf, %s %s";
const char* RECORD_FILE_NAME = "fish_farm_data.txt";

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

// 辅助方法：生成范围内的随机浮点数
float randomInRange(float min, float max) {
    return min + (float)rand() / RAND_MAX * (max - min);
}

// 辅助方法：生成指定类型的时间字符串
void generateTimeStr(char* timeBuf, time_t baseTime, int index) {
    time_t recordTime = baseTime - (30 * (index)); // 每条记录间隔 30 秒
    struct tm* tm_info = localtime(&recordTime);
    strftime(timeBuf, 20, "%Y-%m-%d %H:%M:%S", tm_info);
}

// 用于生成随机数据
struct ArrayList * generateAndSaveRandomWaterQualityData(int count, enum Mode mode) {
    time_t now = time(NULL);
    // 记录的数据间隔为 30s 一条，从这个时间开始记录
    time_t randomTimeStart = now - (30 * count);

    ArrayList list = createAList(count);

    DataRestriction validData = NULL;
    if (mode == PENAEUS_VANNAMEI) {
        validData = penaeusVannameiNormalData;
    } else if (mode == MICROPTERUS_SALMOIDES) {
        validData = micropterusSalmoidesNormalData;
    } else if (mode == CRASSOSTRA_GIGAS) {
        validData = crassostreaGigasNormalData;
    }

    char timeBuf[20];
    for (int i = 0; i < count; i++) {
        struct WaterQuality* quality = (struct WaterQuality*) malloc(sizeof(struct WaterQuality));
        quality->id = i + 1;

        // 在合法范围内生成随机数据
        quality->tmp = randomInRange(validData->minTmp, validData->maxTmp);
        quality->doxygen = randomInRange(validData->minDoxygen, validData->maxDoxygen);
        quality->ammonia = randomInRange(validData->minAmmonia, validData->maxAmmonia);
        quality->ph = randomInRange(validData->minPh, validData->maxPh);

        // 生成时间字符串
        generateTimeStr(timeBuf, randomTimeStart, count - 1 - i);
        strcpy(quality->time, timeBuf);
        randomTimeStart += 30;
        addAList(list, quality);
    }

    return list;
}
