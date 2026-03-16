#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "data_restriction.h"
#include "../proj/data.h"
#include <math.h>
const char* IO_LINE_WRITE_FMT = "%d, %.2f, %.2f, %.2f, %.2f, %s";
const char* IO_LINE_READ_FMT = "%d, %lf, %lf, %lf, %lf, %s %s";
const char* RECORD_FILE_NAME = "fish_farm_data.txt";
const char* RESTRICTION_FILE_NAME = "data_restriction.txt";

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

void readRestrictionData() {
    penaeusVannameiNormalData = (DataRestriction)malloc(sizeof(struct DataRestriction));
    penaeusVannameiSeriousData = (DataRestriction)malloc(sizeof(struct DataRestriction));
    penaeusVannameiValidData = (DataRestriction)malloc(sizeof(struct DataRestriction));
    micropterusSalmoidesNormalData = (DataRestriction)malloc(sizeof(struct DataRestriction));
    micropterusSalmoidesSeriousData = (DataRestriction)malloc(sizeof(struct DataRestriction));
    micropterusSalmoidesValidData = (DataRestriction)malloc(sizeof(struct DataRestriction));
    crassostreaGigasNormalData = (DataRestriction)malloc(sizeof(struct DataRestriction));
    crassostreaGigasSeriousData = (DataRestriction)malloc(sizeof(struct DataRestriction));
    crassostreaGigasValidData = (DataRestriction)malloc(sizeof(struct DataRestriction));

    FILE* f = fopen(RESTRICTION_FILE_NAME, "r");

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
// 辅助方法：生成范围内的随机浮点数
double randomInRange(double min, double max) {
    return min + (double) rand() / RAND_MAX * (max - min);
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
