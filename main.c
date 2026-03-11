#include <stdio.h>

#include "proj/data.h"
#include <stdlib.h>
#include <string.h>
#include "proj/data_restriction.h"
#include "proj/console.h"
#include "lib/array_list.h"
#include "proj/io.h"
const char* file = "fish_farm_data.txt";
void forEachPrintWaterQuality(void* e) {
    WaterQuality quality = (WaterQuality) e;
    printf("id: %d, tmp: %.2f, doxygen: %.2f, ph: %.2f, ammonia: %.2f, time: %s\n",
           quality->id, quality->tmp, quality->doxygen, quality->ph, quality->ammonia, quality->time);
}
void writeTest() {
    printf("11111");
    // 生成5个随机的QaterQuality数据测试
    WaterQuality q1 = malloc(sizeof(struct WaterQuality));
    q1->tmp = 20.0f;
    q1->doxygen = 10.0f;
    q1->ph = 7.0f;
    q1->ammonia = 5.0f;
    q1->id = 1;
    strcpy(q1->time, "2023-03-10 10:10:10");
    WaterQuality q2 = malloc(sizeof(struct WaterQuality));
    q2->tmp = 25.0f;
    q2->doxygen = 15.0f;
    q2->ph = 8.0f;
    q2->ammonia = 10.0f;
    q2->id = 2;
    strcpy(q2->time, "2023-03-10 10:10:10");
    WaterQuality q3 = malloc(sizeof(struct WaterQuality));
    q3->tmp = 30.0f;
    q3->doxygen = 20.0f;
    q3->ph = 9.0f;
    q3->ammonia = 15.0f;
    q3->id = 3;
    strcpy(q3->time, "2023-03-10 10:10:10");
    WaterQuality q4 = malloc(sizeof(struct WaterQuality));
    q4->tmp = 35.0f;
    q4->doxygen = 25.0f;
    q4->ph = 10.0f;
    q4->ammonia = 20.0f;
    q4->id = 4;
    strcpy(q4->time, "2023-03-10 10:10:10");

    WaterQuality q5 = malloc(sizeof(struct WaterQuality));
    q5->tmp = 40.0f;
    q5->doxygen = 30.0f;
    q5->ph = 11.0f;
    q5->ammonia = 25.0f;
    q5->id = 5;
    strcpy(q5->time, "2023-03-10 10:10:10");
    ArrayList list = createAList(5);
    addAList(list, q1);
    addAList(list, q2);
    addAList(list, q3);
    addAList(list, q4);
    addAList(list, q5);
    writeWaterQuality(list, file);
}
void readTest() {
    ArrayList list = readWaterQuality(file);
    forEachAList(list, forEachPrintWaterQuality);
}
int main() {
    writeTest();
    initDataRestriction();
    initConsole();
}
