#ifndef SORT_H
#define SORT_H
// 各种水质质量的排序
int sortById(void* p1, void* p2); // id排序
int sortByTmp(void* p1, void* p2); // 温度排序
int sortByDoxygen(void* p1, void* p2); // 溶解氧排序
int sortByPh(void* p1, void* p2); // PH排序
int sortByAmmonia(void* p1, void* p2); // 氨氮排序
int sortByTime(void* p1, void* p2); // 时间排序
#endif