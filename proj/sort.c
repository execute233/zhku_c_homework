#include "sort.h"
#include "data.h"
#include <string.h>


#define PRECISION 0.001
int sortById(void* p1, void* p2) {
    struct WaterQuality* q1 = (struct WaterQuality*) p1;
    struct WaterQuality* q2 = (struct WaterQuality*) p2;
    return q1->id - q2->id;
}
// 由于精度问题，不能做到绝对的比较
// 返回0代表在指定范围，1表示前者比后者大，反之为-1
static int in(double target, double num, double range) {
    if (num > target - range && num < target + range) return 0;
    return target > num ? 1 : -1;
}
int sortByTmp(void* p1, void* p2) {
    struct WaterQuality* q1 = (struct WaterQuality*) p1;
    struct WaterQuality* q2 = (struct WaterQuality*) p2;
    return in(q1->tmp, q2->tmp, PRECISION);
}
int sortByDoxygen(void* p1, void* p2) {
    struct WaterQuality* q1 = (struct WaterQuality*) p1;
    struct WaterQuality* q2 = (struct WaterQuality*) p2;
    return in(q1->doxygen, q2->doxygen, PRECISION);
}
int sortByPh(void* p1, void* p2) {
    struct WaterQuality* q1 = (struct WaterQuality*) p1;
    struct WaterQuality* q2 = (struct WaterQuality*) p2;
    return in(q1->ph, q2->ph, PRECISION);
}
int sortByAmmonia(void* p1, void* p2) {
    struct WaterQuality* q1 = (struct WaterQuality*) p1;
    struct WaterQuality* q2 = (struct WaterQuality*) p2;
    return in(q1->ammonia, q2->ammonia, PRECISION);
}
int sortByTime(void* p1, void* p2) {
    struct WaterQuality* q1 = (struct WaterQuality*) p1;
    struct WaterQuality* q2 = (struct WaterQuality*) p2;
    return strcmp(q1->time, q2->time);
}