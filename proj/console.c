#include "console.h"
#include <stdio.h>
#include <stdarg.h>
#include <conio.h>
#include <windows.h>

#include "data.h"
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
    NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9, DOT
    , UP, DOWN, LEFT, RIGHT, ENTER, BACKSPACE, DEL, SPACE, ESC
    , UNKOWN
};
// 针对windows终端的控制
void initTerminal() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }

    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 100;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

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
            case '0':
                return NUM0;
            case '1':
                return NUM1;
            case '2':
                return NUM2;
            case '3':
                return NUM3;
            case '4':
                return NUM4;
            case '5':
                return NUM5;
            case '6':
                return NUM6;
            case '7':
                return NUM7;
            case '8':
                return NUM8;
            case '9':
                return NUM9;
            case '.':
            case 161:
                return DOT;
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
const char* WATCH_END_TIPS = "%d / %d Pages  使用方向键以切换页码，ESC/backspce 以退出";
const char* EDIT_END_TIPS = "%d / %d Pages  使用方向键以切换页码，ESC/backspce 以退出，按下Enter键进入编辑，再次按下Enter键以保存，ESC/Backspace键不保存";
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
        printf(WATCH_END_TIPS, page + 1, maxPage + 1);
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
void gotoxy(int x, int y) {
    COORD pos;
    pos.X = x;
    pos.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}
// 截取ArrayList的某个范围里的水质数据添加到新的ArrayList字符串里
// 替换找到的第i个指定字符后面的第j个字符为新的字符
void replaceTargetCharOnNOChar(char* str, char target, int i, int j, char newChar) {
    for (int k = 0; str[k] != '\0'; k++) {
        if (str[k] == target) {
            i--;
        }
        if (i == 0) {
            str[k + j] = newChar;
            return;
        }
    }
}
void doReplace(char* str, int cursorX , char target) {
    const int startIndex1 = 0, startIndex2 = 16, startIndex3 = 32, startIndex4 = 40, startIndex5 = 56, startIndex6 = 67;
    const int endIndex1 = startIndex1 + 4, endIndex2 = startIndex2 + 4, endIndex3 = startIndex3 + 4, endIndex4 = startIndex4 + 3, endIndex5 = startIndex5 + 9, endIndex6 = startIndex6 + 9;
    if (cursorX >= startIndex1 && cursorX <= endIndex1) {
        replaceTargetCharOnNOChar(str, '\t', 1, cursorX - startIndex1 + 1, target);
    }
    if (cursorX >= startIndex2 && cursorX <= endIndex2) {
        replaceTargetCharOnNOChar(str, '\t', 3, cursorX - startIndex2 + 1, target);
    }
    if (cursorX >= startIndex3 && cursorX <= endIndex3) {
        replaceTargetCharOnNOChar(str, '\t', 5, cursorX - startIndex3 + 1, target);
    }
    if (cursorX >= startIndex4 && cursorX <= endIndex4) {
        replaceTargetCharOnNOChar(str, '\t', 6, cursorX - startIndex4 + 1, target);
    }
    if (cursorX >= startIndex5 && cursorX <= endIndex6) {
        replaceTargetCharOnNOChar(str, '\t', 8, cursorX - startIndex5 + 1, target);
    }
}
// 左右光标快速跳转，把一些没用的空行直接跳了
int quickJump(int cursorX, bool jumpLeft) {
    const int startIndex1 = 0, startIndex2 = 16, startIndex3 = 32, startIndex4 = 40, startIndex5 = 56, startIndex6 = 67;
    const int endIndex1 = startIndex1 + 4, endIndex2 = startIndex2 + 4, endIndex3 = startIndex3 + 4, endIndex4 = startIndex4 + 3, endIndex5 = startIndex5 + 9, endIndex6 = startIndex6 + 9;
    if (jumpLeft) {
        if (startIndex1 == cursorX) return endIndex6;
        if (startIndex2 == cursorX) return endIndex1;
        if (startIndex3 == cursorX) return endIndex2;
        if (startIndex4 == cursorX) return endIndex3;
        if (startIndex5 == cursorX) return endIndex4;
        if (startIndex6 == cursorX) return endIndex5;
        return cursorX - 1;
    }
    if (endIndex1 == cursorX) return startIndex2;
    if (endIndex2 == cursorX) return startIndex3;
    if (endIndex3 == cursorX) return startIndex4;
    if (endIndex4 == cursorX) return startIndex5;
    if (endIndex5 == cursorX) return startIndex6;
    if (endIndex6 == cursorX) return startIndex1;
    return cursorX + 1;

}
ArrayList cpyAList(ArrayList list, int startIndex, int len) {
    ArrayList result = createAList(len);
    for (int i = startIndex; i < startIndex + len; i++) {
        char* str = malloc(sizeof(char) * 128);
        struct WaterQuality * quality = getAList(list, i);
        sprintf(str, PRINT_WATER_QUALITY_FMT, quality->id, quality->tmp, quality->doxygen, quality->ph, quality->ammonia, quality->time);
        addAList(result, str);
    }
    return result;
}
// 修改历史数据
void editHistoryRecord() {
    const int showRowsCount = getVisibleRows() - 2;
    int page = 0;
    int maxPage = globalRecordList->size % showRowsCount == 0 ? globalRecordList->size / showRowsCount : globalRecordList->size / showRowsCount + 1;
    int currentCursorX = 0, currentCursorY = 0;
    int cursorMinX = 0, cursorMaxX = 74, cursorMinY = 0, cursorMaxY = showRowsCount - 1;
    const int posStartX = 8, posStartY = 1;
    // 一些数值定义的相对位置范围[]
    bool edit = false;
    // 缓存当前页的数据用于修改
    ArrayList bufList = createAList(showRowsCount);
    while (true) {
        if (edit) {
            printf(SHOW_CURSOR);
        } else {
            printf(HIDE_CURSOR);
        }
        printDefaultAutoEnter(INFO);
        int startIndex = page * showRowsCount;
        int endIndex = startIndex + showRowsCount - 1;
        if (endIndex >= globalRecordList->size) {
            endIndex = globalRecordList->size - 1;
        }
        // 输出水质数据
        if (!edit) {
            for (int i = startIndex; i <= endIndex; i++) {
                printWaterQualityAutoEnter(getAList(globalRecordList, i));
            }
        } else {
            for (int i = 0; i < bufList->size; i++) {
                printDefaultAutoEnter(getAList(bufList, i));
            }
        }

        printf(EDIT_END_TIPS, page + 1, maxPage + 1);
        // 光标要在输出完成后移动
        gotoxy(posStartX + currentCursorX, posStartY + currentCursorY);
        enum KeyType key = waitForAnyKey(18, UP, DOWN, LEFT, RIGHT, ESC, BACKSPACE, ENTER
            , NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9, DOT);


        switch (key) {
            case BACKSPACE:
            case ESC:
                if (edit) {
                    edit = false;
                } else {
                    return;
                }
            case ENTER:
                if (edit) {
                    edit = false;
                } else {
                    clearAListRls(bufList);
                    // 将当前页的数据缓存起来
                    bufList = cpyAList(globalRecordList, startIndex, showRowsCount);
                    edit = true;
                }
            case UP:
                if (edit) {
                    currentCursorY = currentCursorY == cursorMinY ? cursorMinY : currentCursorY - 1;
                }
                break;
            case DOWN:
                if (edit) {
                    currentCursorY = currentCursorY == cursorMaxY ? cursorMaxY : currentCursorY + 1;
                }
                break;
            case LEFT:
                if (edit) {
                    currentCursorX = currentCursorX == cursorMinX ? cursorMinX : quickJump(currentCursorX, true);
                } else {
                    page = page == 0 ? 0 : page - 1;
                }
                break;
            case RIGHT:
                if (edit) {
                    currentCursorX = currentCursorX == cursorMaxX ? cursorMaxX : quickJump(currentCursorX, false);
                } else {
                    page = page == maxPage ? maxPage : page + 1;
                }
                break;
            case NUM0:
            case NUM1:
            case NUM2:
            case NUM3:
            case NUM4:
            case NUM5:
            case NUM6:
            case NUM7:
            case NUM8:
            case NUM9:
            case DOT:
                if (edit) {
                    char* str = getAList(bufList, currentCursorY);
                    if (key == DOT) {
                        doReplace(str, currentCursorX, '.');
                        break;
                    }
                    doReplace(str, currentCursorX, '0' + key);
                }
                break;
            default: ;
        }
        clearScreen();
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
                        editHistoryRecord();
                        break;
                case 4:

                        break;
                case 5:
                        writeWaterQualityRecords(globalRecordList);
                        printDefaultAutoEnter("ByeBye!");
                        return;
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
    globalRecordList = readWaterQualityRecords();
    initTerminal();
    chooseModeInit();

    userLoopInit();
    editHistoryRecord();
    printf(SHOW_CURSOR);
}