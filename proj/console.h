#ifndef CONSOLE_H
#define CONSOLE_H
#include "../lib/array_list.h"
#include "data.h"
#include <stdbool.h>

// 指示选择的养殖类型
enum Mode {
    PENAEUS_VANNAMEI, // 南美对白虾
    MICROPTERUS_SALMOIDES, // 大口黑鲈
    CRASSOSTRA_GIGAS // 南澳牡蛎
};

enum KeyType {
    NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9, DOT,
    UP, DOWN, LEFT, RIGHT, ENTER, BACKSPACE, DEL, SPACE, ESC
};

extern struct ArrayList* globalRecordList;
extern enum Mode mode;

void initTerminal();
void exitTerminal();
int kbhit();
int getKey();
enum KeyType waitForKey();
void waitForRightKey(enum KeyType type);
enum KeyType waitForAnyKey(int count, ...);
int getVisibleRows();
void printDefaultAutoEnter(char* format, ...);
void printfWhiteBkgAutoEnter(char* format, ...);
void printfWhileBkgBoolAutoEnter(bool condition, char* format, ...);
void clearScreen();
void gotoxy(int x, int y);
void printWaterQualityAutoEnter(struct WaterQuality* q);
void printWaterQualityWhileBkgAutoEnter(struct WaterQuality* q);

void chooseModeInit();
void userLoopInit();
void initConsole();

void seeHistoryRecord();
void editHistoryRecord();
void delHistoryRecord();
void watchInit();
void seeStatistics();
void manageUsers();

//新增
void changePassword();
static void input_password(char* pwd, int max_len);

#endif