#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include "../proj/data.h"
#include <string.h>
const char* IO_LINE_WRITE_FMT = "%d, %.2f, %.2f, %.2f, %.2f, %s";
const char* IO_LINE_READ_FMT = "%d, %f, %f, %f, %f, %s %s";

// 写入到文件，如果文件存在则会清空内容写入
void writeWaterQuality(ArrayList list, char* file) {
    FILE* f = fopen(file, "w");
    for (int i = 0; i < list->size; i++) {
        WaterQuality quality = getAList(list, i);
        fprintf(f, IO_LINE_WRITE_FMT, quality->id, quality->tmp, quality->doxygen, quality->ph, quality->ammonia, quality->time);
        fprintf(f, "\n");
    }
    fflush(f);
    fclose(f);
}
// 读取文件，构建为ArrayList
ArrayList readWaterQuality(char* file) {
    FILE* f = fopen(file, "r");
    ArrayList list = createAListDefault();
    while (true) {
        WaterQuality quality = malloc(sizeof(struct WaterQuality));
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
