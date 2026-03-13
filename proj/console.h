#ifndef CONSOLE_H
#define CONSOLE_H
#include "../lib/array_list.h"

// 指示选择的养殖类型
enum Mode {
    PENAEUS_VANNAMEI, // 南美对白虾
    MICROPTERUS_SALMOIDES, // 大口黑鲈
    CRASSOSTRA_GIGAS // 南澳牡蛎
};
extern struct ArrayList * globalRecordList;
extern enum Mode mode;
void initConsole();
void printDefaultAutoEnter(char* format, ...);
void printfWhiteBkgAutoEnter(char* format, ...);
void printfRedBkgAutoEnter(char* format, ...);
void clearScreen();
#endif