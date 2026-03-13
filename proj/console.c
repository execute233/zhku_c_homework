#include "console.h"
#include <stdio.h>
#include <stdarg.h>
#include <conio.h>
#include <windows.h>

#include "data_restriction.h"
#include "io.h"
#define CLEAR_SCREEN "\033[2J"
#define MOVE_CURSOR(y, x) "\033[" #y ";" #x "H"
#define COLOR_RED  "\033[31m"
#define COLOR_GREEN  "\033[32m"
#define COLOR_BOLD  "\033[1m"
#define RESET  "\033[0m"
#define HIDE_CURSOR  "\033[?25l"
#define SHOW_CURSOR  "\033[?25h"
#define WHITE_BG     "\033[47m"
#define BLACK_FG     "\033[30m"
#define BOLD         "\033[1m"
#define STYLE_INVERTED WHITE_BG BLACK_FG
#define CLEAR_AND_HOME "\033[2J\033[H"
#define CLEAR_FULL "\033[2J\033[3J\033[H"
enum Mode mode = PENAEUS_VANNAMEI;
struct ArrayList * globalRecordList = NULL;
// 操作的按键
enum KeyType {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    ENTER,
    BACKSPACE,
    DEL,
    SPACE,
    ESC,
    UNKOWN
};
// 针对windows终端的控制
void initTerminal() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }
    printf(HIDE_CURSOR);
}
int kbhit() {
    return _kbhit();
}
int getKey() {
    return _getch();
}
enum KeyType waitForKey() {
    while (true) {
        int part1 = getKey();
        if (part1 == 0 || part1 == 224) {
            switch (getKey()) {
                case 72:
                    return UP;
                case 80:
                    return DOWN;
                case 77:
                    return RIGHT;
                case 75:
                    return LEFT;
                case 83:
                    return DEL;
                default: continue;
            }
        }
        switch (part1) {
            case 27:
                return ESC;
            case 13:
                return  ENTER;
            case 8:
                return  BACKSPACE;
            case 32:
                return SPACE;
            default: continue;
        }
    }
}
void waitForRightKey(enum KeyType type) {
    while (true) {
        if (waitForKey() == type) {
            return;
        }
    }
}
enum KeyType waitForAnyKey(int count, ...) {
    va_list args;
    va_start(args, count);
    int keys[count];
    for (int i = 0; i < count; i++) {
        keys[i] = va_arg(args, int);
    }
    while (true) {
        enum KeyType key = waitForKey();
        for (int i = 0; i < count; i++) {
            if (keys[i] == key) {
                return key;
            }
        }
    }
}
void clearScreen() {
    printf(CLEAR_FULL);
}
int getVisibleRows() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
    return 15; // 默认15
}

void printDefaultAutoEnter(char* format, ...) {
    va_list args;
    va_start(args, format);
    printf(format, args);
    va_end(args);
    printf("\n");
}
// 输出高亮文本，自动换行
void printfWhiteBkgAutoEnter(char* format, ...) {
    va_list args;
    va_start(args, format);
    printf(STYLE_INVERTED);
    printf(format, args);
    va_end(args);
    printf(RESET);
    printf("\n");
}
// 条件输出高亮文本，自动换行
void printfWhileBkgBoolAutoEnter(bool condition, char* format, ...) {
    va_list args;
    va_start(args, format);
    if (condition) {
        printf(STYLE_INVERTED);
        printf(format, args);
        printf(RESET);
        printf("\n");
    } else {
        printf(format, args);
        printf("\n");
    }
    va_end(args);
}
const char* INFO = "id\t水温（度）\t溶解氧(mg/L)\tPH\t氨氮(mg/L)\t时间";
const char* END_TIPS = "按上下左右以切换方向，ESC/backspce 以退出";
const char* PRINT_WATER_QUALITY_FMT = "%d\t%.2f\t\t%.2f\t\t%.2f\t%.2f\t\t%s";
void printWaterQualityAutoEnter(struct WaterQuality * quality) {
    enum RestrictionType restriction;
    if (mode == PENAEUS_VANNAMEI) {
        restriction = checkPenaeusVannameiData(quality);
    } else if (mode == MICROPTERUS_SALMOIDES) {
        restriction = checkMicropterusSalmoidesData(quality);
    } else {
        restriction = checkCrassostreaGigasData(quality);
    }
    char printStr[128];
    sprintf(printStr, PRINT_WATER_QUALITY_FMT, quality->id, quality->tmp, quality->doxygen, quality->ph, quality->ammonia, quality->time);
    if (restriction == NORMAL) {
        printDefaultAutoEnter(printStr);
    } else if (restriction == NORMAL_ALERT) {
        printDefaultAutoEnter(printStr);
    } else if (restriction == SERIOUS_ALERT) {
        printDefaultAutoEnter(printStr);
    } else {
        printDefaultAutoEnter(printStr);
    }
}
// 选择养殖类型
void chooseModeInit() {
    int choose = 1;
    while (true) {
        printDefaultAutoEnter("欢迎使用水产养殖系统！v1.0");
        printDefaultAutoEnter("请选择养殖类型：（按下Enter确定）");
        printfWhileBkgBoolAutoEnter(choose == 1, "南美白对虾");
        printfWhileBkgBoolAutoEnter(choose == 2, "大口黑鲈");
        printfWhileBkgBoolAutoEnter(choose == 3, "南奥牡蛎");
        enum KeyType key = waitForAnyKey(3, UP, DOWN, ENTER);
        clearScreen();
        switch (key) {
            case UP:
                choose = choose == 1 ? 3 : choose - 1;
                break;
            case DOWN:
                choose = choose == 3 ? 1 : choose + 1;
                break;
            case ENTER:
                switch (choose) {
                case 1:
                        mode = PENAEUS_VANNAMEI;
                        break;
                case 2:
                        mode = MICROPTERUS_SALMOIDES;
                        break;
                case 3:
                        mode = CRASSOSTRA_GIGAS;
                        break;
                default:
                        continue;
                }
                clearScreen();
                return;
            default: continue;
        }
    }
}
// 查看历史数据
void seeHistoryRecord() {
    int showRowsCount = getVisibleRows() - 2;
    int page = 0;
    int maxPage = globalRecordList->size % showRowsCount == 0 ? globalRecordList->size / showRowsCount : globalRecordList->size / showRowsCount + 1;
    while (true) {
        printDefaultAutoEnter(INFO);
        int startIndex = page * showRowsCount;
        int endIndex = startIndex + showRowsCount - 1;
        if (endIndex >= globalRecordList->size) {
            endIndex = globalRecordList->size - 1;
        }
        for (int i = startIndex; i <= endIndex; i++) {
            printWaterQualityAutoEnter(getAList(globalRecordList, i));
        }
        printf(END_TIPS);
        enum KeyType key = waitForAnyKey(6, UP, DOWN, LEFT, RIGHT, ESC, BACKSPACE);

        clearScreen();
        switch (key) {
            case BACKSPACE:
                return;
            case ESC:
                return;
            case UP:
                // page = page == 0 ? 0 : page - 1;
                break;
            case DOWN:
                //  page = page + 1;
                break;
            case LEFT:
                page = page == 0 ? 0 : page - 1;
                break;
            case RIGHT:
                page = page == maxPage ? maxPage : page + 1;
                break;
            default: ;
        }
    }
}
// 开始监测逻辑
void watchInit() {
    printDefaultAutoEnter("开始监测...");
    printDefaultAutoEnter("[生成随机数据模拟监测]");
    clearAListRls(globalRecordList);
    free(globalRecordList);
    globalRecordList = generateAndSaveRandomWaterQualityData( 1000, mode);
    printDefaultAutoEnter("监测完毕！数据已保存，按下任意键返回");
    getKey();
}
void userLoopInit() {
    globalRecordList = createAListDefault();
    int choose = 1;
    while (true) {
        printfWhileBkgBoolAutoEnter(choose == 1, "开始监测");
        printfWhileBkgBoolAutoEnter(choose == 2, "查看历史数据");
        printfWhileBkgBoolAutoEnter(choose == 3, "修改历史数据");
        printfWhileBkgBoolAutoEnter(choose == 4, "删除历史数据");
        printfWhileBkgBoolAutoEnter(choose == 5, "退出并保存配置");
        enum KeyType key = waitForAnyKey(3, UP, DOWN, ENTER);
        clearScreen();
        switch (key) {
            case UP:
                choose = choose == 1 ? 5 : choose - 1;
                break;
            case DOWN:
                choose = choose == 5 ? 1 : choose + 1;
                break;
            case ENTER:
                switch (choose) {
                case 1:
                        watchInit();
                        break;
                case 2:
                        seeHistoryRecord();
                        break;
                case 3:

                        break;
                case 4:

                        break;
                case 5:
                        printDefaultAutoEnter("ByeBye!");
                        break;
                default:
                        continue;
                }
                clearScreen();
            default: continue;
        }
        clearScreen();
    }
}
void initConsole() {
    initTerminal();
    chooseModeInit();
    userLoopInit();
    printf(SHOW_CURSOR);
}