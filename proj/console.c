#include "console.h"
#include "data_restriction.h"
#include "io.h"
#include "sort.h"
#include "user_management.h"
#include <stdio.h>
#include <stdarg.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#define TAB          "\t"
#define DOUBLE_TAB   "\t\t"
#define DOUBLE_FMT   "%lf"
#define DOUBLE_FMT2  "%.2lf"
#define CLEAR_SCREEN "\033[2J"
#define MOVE_CURSOR(y, x) "\033[" #y ";" #x "H"
#define COLOR_RED    "\033[31m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_WHITE  "\033[37m"
#define COLOR_GRAY   "\033[38;5;245m"
#define RESET        "\033[0m"
#define HIDE_CURSOR  "\033[?25l"
#define CURSOR_BLOCK_BLINK   "\033[1 q"
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

static u_int beforeConsoleOutputCP;
// 针对windows终端的控制
void initTerminal() {
    beforeConsoleOutputCP = GetConsoleOutputCP();
    SetConsoleOutputCP(65001);
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
void exitTerminal() {
    printf(SHOW_CURSOR);
    SetConsoleOutputCP(beforeConsoleOutputCP);
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
static const char* INFO = "id\t水温（度）\t溶解氧(mg/L)\tPH\t氨氮(mg/L)\t时间";
static const char* SEE_END_TIPS = "%d / %d Pages  使用方向键以切换页码，ESC/backspce 以退出，排序方式:%s\n1:id 2:温度 3:含氧量 4:ph 5:氨氮量 6:时间";
static const char* EDIT_END_TIPS = "%d / %d Pages  使用方向键以切换页码，ESC/backspce 以退出\n按下Enter键进入编辑，再次按下Enter键以保存，ESC/Backspace键不保存";
static const char* DEL_END_TIPS = "%d / %d Pages  使用方向键以切换页码，ESC/backspce 以退出，Del键以删除选择的行";
static const char* PRINT_WATER_QUALITY_FMT = "%d\t%.2lf\t\t%.2lf\t\t%.2lf\t%.2lf\t\t%s";
static const char* PRINT_WQ_WITH_COLOR_FMT = "%d\t%s%.2lf%s\t\t%s%.2lf%s\t\t%s%.2lf%s\t%s%.2lf%s\t\t%s";
static const char* STR_TO_WQ_FMT = "%d\t%lf\t\t%lf\t\t%lf\t%lf\t\t%s%s";
static const int START_INDEX_1 = 0, START_INDEX_2 = 16, START_INDEX_3 = 32, START_INDEX_4 = 40, START_INDEX_5 = 56, START_INDEX_6 = 67;
static const int END_INDEX_1 = START_INDEX_1 + 4, END_INDEX_2 = START_INDEX_2 + 4, END_INDEX_3 = START_INDEX_3 + 4, END_INDEX_4 = START_INDEX_4 + 3, END_INDEX_5 = START_INDEX_5 + 9, END_INDEX_6 = START_INDEX_6 + 9;
static const char* STR_TO_TIME_FMT ="%d-%d-%d %d:%d:%d";

static char printBuf[128 * 64 * 2];
void clearScreen() {
    printBuf[0] = '\0';
    printf(CLEAR_FULL);
}

// 下面方法直接输出，不输出到输出缓存里
void printWaterQualityAutoEnter(struct WaterQuality* quality) {
    struct DataRestriction* normalRestriction;
    struct DataRestriction* seriousRestriction;
    struct DataRestriction* validRestriction;
    char buf[128 * 2];
    if (mode == PENAEUS_VANNAMEI) {
        normalRestriction = penaeusVannameiNormalData;
        seriousRestriction = penaeusVannameiSeriousData;
        validRestriction = penaeusVannameiValidData;
    } else if (mode == MICROPTERUS_SALMOIDES) {
        normalRestriction = micropterusSalmoidesNormalData;
        seriousRestriction = micropterusSalmoidesSeriousData;
        validRestriction = micropterusSalmoidesValidData;
    } else {
        normalRestriction = crassostreaGigasNormalData;
        seriousRestriction = crassostreaGigasSeriousData;
        validRestriction = crassostreaGigasValidData;
    }
    char* tmpColor;
    char* doxygenColor;
    char* phColor;
    char* ammoniaColor;
    // 温度
    if (quality->tmp <= normalRestriction->maxTmp && quality->tmp >= normalRestriction->minTmp) {
        tmpColor = COLOR_WHITE;
    } else if (quality->tmp <= seriousRestriction->maxTmp && quality->tmp >= seriousRestriction->minTmp) {
        tmpColor = COLOR_YELLOW;
    } else if (quality->tmp <= validRestriction->maxTmp && quality->tmp >= validRestriction->minTmp) {
        tmpColor = COLOR_RED;
    } else {
        tmpColor = COLOR_GRAY;
    }
    // 含氧量
    if (quality->doxygen <= normalRestriction->maxDoxygen && quality->doxygen >= normalRestriction->minDoxygen) {
        doxygenColor = COLOR_WHITE;
    } else if (quality->doxygen <= seriousRestriction->maxDoxygen && quality->doxygen >= seriousRestriction->minDoxygen) {
        doxygenColor = COLOR_YELLOW;
    } else if (quality->doxygen <= validRestriction->maxDoxygen && quality->doxygen >= validRestriction->minDoxygen) {
        doxygenColor = COLOR_RED;
    } else {
        doxygenColor = COLOR_GRAY;
    }
    // PH
    if (quality->ph <= normalRestriction->maxPh && quality->ph >= normalRestriction->minPh) {
        phColor = COLOR_WHITE;
    } else if (quality->ph <= seriousRestriction->maxPh && quality->ph >= seriousRestriction->minPh) {
        phColor = COLOR_YELLOW;
    } else if (quality->ph <= validRestriction->maxPh && quality->ph >= validRestriction->minPh) {
        phColor = COLOR_RED;
    } else {
        doxygenColor = COLOR_GRAY;
    }
    // 氨氮含量
    if (quality->ammonia <= normalRestriction->maxAmmonia && quality->ammonia >= normalRestriction->minAmmonia) {
        ammoniaColor = COLOR_WHITE;
    } else if (quality->ammonia <= seriousRestriction->maxAmmonia && quality->ammonia >= seriousRestriction->minAmmonia) {
        ammoniaColor = COLOR_YELLOW;
    } else if (quality->ammonia <= validRestriction->maxAmmonia && quality->ammonia >= validRestriction->minAmmonia) {
        ammoniaColor = COLOR_RED;
    } else {
        doxygenColor = COLOR_GRAY;
    }
    sprintf(buf, PRINT_WQ_WITH_COLOR_FMT, quality->id, tmpColor, quality->tmp, COLOR_WHITE, doxygenColor, quality->doxygen, COLOR_WHITE, phColor, quality->ph, COLOR_WHITE, ammoniaColor, quality->ammonia, COLOR_WHITE, quality->time);
    printDefaultAutoEnter(buf);
}
void printWaterQualityWhileBkgAutoEnter(struct WaterQuality * quality) {
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
        printfWhiteBkgAutoEnter(printStr);
    } else if (restriction == NORMAL_ALERT) {
        printfWhiteBkgAutoEnter(printStr);
    } else if (restriction == SERIOUS_ALERT) {
        printfWhiteBkgAutoEnter(printStr);
    } else {
        printfWhiteBkgAutoEnter(printStr);
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
    int sortType = 1; // 排序方式，最开始看是没排序的，从文件怎样读就怎样顺序
    bool sortEsc = false; // 升序
    int showRowsCount = getVisibleRows() - 3;
    int page = 0;
    int maxPage = globalRecordList->size % showRowsCount == 0 ? globalRecordList->size / showRowsCount - 1 : globalRecordList->size / showRowsCount;
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
        printf(SEE_END_TIPS, page + 1, maxPage + 1, sortEsc ? "升序" : "降序");
        enum KeyType key = waitForAnyKey(12
            , UP, DOWN, LEFT, RIGHT, ESC, BACKSPACE, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6);

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
            case NUM1:
                if (sortType == 1) {
                    sortEsc = !sortEsc;
                } else {
                    sortType = 1;
                }
                sort(globalRecordList, sortEsc, sortById);
                break;
            case NUM2:
                if (sortType == 2) {
                    sortEsc = !sortEsc;
                } else {
                    sortType = 2;
                }
                sort(globalRecordList, sortEsc, sortByTmp);
                break;
            case NUM3:
                if (sortType == 3) {
                    sortEsc = !sortEsc;
                } else {
                    sortType = 3;
                }
                sort(globalRecordList, sortEsc, sortByDoxygen);
                break;
            case NUM4:
                if (sortType == 4) {
                    sortEsc = !sortEsc;
                } else {
                    sortType = 4;
                }
                sort(globalRecordList, sortEsc, sortByPh);
                break;
            case NUM5:
                if (sortType == 5) {
                    sortEsc = !sortEsc;
                } else {
                    sortType = 5;
                }
                sort(globalRecordList, sortEsc, sortByAmmonia);
                break;
            case NUM6:
                if (sortType == 6) {
                    sortEsc = !sortEsc;
                } else {
                    sortType = 6;
                }
                sort(globalRecordList, sortEsc, sortByTime);
                break;
            default: ;
        }
        clearScreen();
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
    if (cursorX >= START_INDEX_1 && cursorX <= END_INDEX_1) {
        replaceTargetCharOnNOChar(str, '\t', 1, cursorX - START_INDEX_1 + 1, target);
    }
    if (cursorX >= START_INDEX_2 && cursorX <= END_INDEX_2) {
        replaceTargetCharOnNOChar(str, '\t', 3, cursorX - START_INDEX_2 + 1, target);
    }
    if (cursorX >= START_INDEX_3 && cursorX <= END_INDEX_3) {
        replaceTargetCharOnNOChar(str, '\t', 5, cursorX - START_INDEX_3 + 1, target);
    }
    if (cursorX >= START_INDEX_4 && cursorX <= END_INDEX_4) {
        replaceTargetCharOnNOChar(str, '\t', 6, cursorX - START_INDEX_4 + 1, target);
    }
    if (cursorX >= START_INDEX_5 && cursorX <= END_INDEX_6) {
        replaceTargetCharOnNOChar(str, '\t', 8, cursorX - START_INDEX_5 + 1, target);
    }
}
// 左右光标快速跳转，把一些没用的空行直接跳了
int quickJump(int cursorX, bool jumpLeft) {
    if (jumpLeft) {
        if (START_INDEX_1 == cursorX) return END_INDEX_6;
        if (START_INDEX_2 == cursorX) return END_INDEX_1;
        if (START_INDEX_3 == cursorX) return END_INDEX_2;
        if (START_INDEX_4 == cursorX) return END_INDEX_3;
        if (START_INDEX_5 == cursorX) return END_INDEX_4;
        if (START_INDEX_6 == cursorX) return END_INDEX_5;
        return cursorX - 1;
    }
    if (END_INDEX_1 == cursorX) return START_INDEX_2;
    if (END_INDEX_2 == cursorX) return START_INDEX_3;
    if (END_INDEX_3 == cursorX) return START_INDEX_4;
    if (END_INDEX_4 == cursorX) return START_INDEX_5;
    if (END_INDEX_5 == cursorX) return START_INDEX_6;
    if (END_INDEX_6 == cursorX) return START_INDEX_1;
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
// 确认编辑的信息，并检查
bool confirmEdit(ArrayList list, int index) {
    ArrayList qualityList = createAList(list->size);
    for (int i = 0; i < list->size; i++) {
        char* str = getAList(list, i);
        struct WaterQuality* quality = malloc(sizeof(struct WaterQuality));
        char rightTime[10];
        int scanCount = sscanf(str, STR_TO_WQ_FMT, &quality->id, &quality->tmp, &quality->doxygen, &quality->ph, &quality->ammonia, quality->time, rightTime);
        if (scanCount != 7) {
            free(quality);
            destroyAListRls(qualityList);
            return false;
        }
        if (mode == CRASSOSTRA_GIGAS) {
            if (checkCrassostreaGigasData(quality) == INVALID_DATA) {
                free(quality);
                destroyAListRls(qualityList);
                return false;
            }
        }
        if (mode == MICROPTERUS_SALMOIDES) {
            if (checkMicropterusSalmoidesData(quality) == INVALID_DATA) {
                free(quality);
                destroyAListRls(qualityList);
                return false;
            }
        }
        if (mode == PENAEUS_VANNAMEI) {
            if (checkPenaeusVannameiData(quality) == INVALID_DATA) {
                free(quality);
                destroyAListRls(qualityList);
                return false;
            }
        }
        strcat(quality->time, " ");
        strcat(quality->time, rightTime);

        // 时间验证
        time_t now = time(NULL);
        struct tm tm = {0};
        int year, month, day, hour, min, sec;
        // 假设格式固定为 "YYYY-MM-DD HH:MM:SS"
        // 如果格式不同，请修改 sscanf 的格式串
        if (sscanf(quality->time, STR_TO_TIME_FMT, &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec) != 6) {
            free(quality);
            destroyAListRls(qualityList);
            return false;
        }
        tm.tm_year -= 1900; // 年份从 1900 开始计数
        tm.tm_mon -= 1; // 月份从 0 (一月) 到 11 (十二月)
        tm.tm_isdst = -1; // 让系统自动判断是否夏令时
        time_t t = mktime(&tm);
        if (t == -1 || t > now) {
            free(quality);
            destroyAListRls(qualityList);
            return false;
        }

        addAList(qualityList, quality);
    }
    for (int i = 0; i < qualityList->size; i++) {
        replaceAListRls(globalRecordList, i + index, getAList(qualityList, i));
    }
    destroyAList(qualityList);
    return true;
}
// 修改历史数据
void editHistoryRecord() {
    bool needFlush = true;
    const int showRowsCount = getVisibleRows() - 3;
    int page = 0;
    int maxPage = globalRecordList->size % showRowsCount == 0 ? globalRecordList->size / showRowsCount - 1 : globalRecordList->size / showRowsCount;
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
            printf(CURSOR_BLOCK_BLINK);
        } else {
            needFlush = true;
            printf(HIDE_CURSOR);
        }
        if (needFlush) {
            printDefaultAutoEnter(INFO);
        }
        int startIndex = page * showRowsCount;
        int endIndex = startIndex + showRowsCount - 1;
        if (endIndex >= globalRecordList->size) {
            endIndex = globalRecordList->size - 1;
        }
        cursorMaxY = endIndex - startIndex;
        // 输出水质数据
        if (needFlush) {
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
        }
        // 光标要在输出完成后移动
        gotoxy(posStartX + currentCursorX, posStartY + currentCursorY);
        enum KeyType key = waitForAnyKey(18, UP, DOWN, LEFT, RIGHT, ESC, BACKSPACE, ENTER
            , NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9, DOT);


        switch (key) {
            case BACKSPACE:
            case ESC:
                if (edit) {
                    needFlush = true;
                    edit = false;
                } else {
                    return;
                }
                break;
            case ENTER:
                if (edit) {
                    needFlush = true;
                    bool confirm_edit = confirmEdit(bufList, startIndex);
                    clearScreen();
                    if (confirm_edit) {
                        printDefaultAutoEnter("编辑成功！");
                    } else {
                        printDefaultAutoEnter("输入的数据有误!");
                    }
                    Sleep(3000);
                    edit = false;
                } else {
                    needFlush = true;
                    clearAListRls(bufList);
                    // 将当前页的数据缓存起来
                    bufList = cpyAList(globalRecordList, startIndex, endIndex - startIndex + 1);
                    edit = true;
                }
                break;
            case UP:
                if (edit) {
                    needFlush = false;
                    currentCursorY = currentCursorY == cursorMinY ? cursorMinY : currentCursorY - 1;
                }
                break;
            case DOWN:
                if (edit) {
                    needFlush = false;
                    currentCursorY = currentCursorY == cursorMaxY ? cursorMaxY : currentCursorY + 1;
                }
                break;
            case LEFT:
                if (edit) {
                    needFlush = false;
                    currentCursorX = currentCursorX == cursorMinX ? cursorMinX : quickJump(currentCursorX, true);
                } else {
                    needFlush = true;
                    page = page == 0 ? 0 : page - 1;
                }
                break;
            case RIGHT:
                if (edit) {
                    needFlush = false;
                    currentCursorX = currentCursorX == cursorMaxX ? cursorMaxX : quickJump(currentCursorX, false);
                } else {
                    needFlush = true;
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
                    needFlush = true;
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
        if (needFlush) {
            clearScreen();
        }
    }
}
// 删除历史数据
void delHistoryRecord() {
    int showRowsCount = getVisibleRows() - 2;
    int page = 0, chooseRowIndex = 0, maxChoosRowIndex = showRowsCount - 1;
    int maxPage = globalRecordList->size % showRowsCount == 0 ? globalRecordList->size / showRowsCount - 1 : globalRecordList->size / showRowsCount;
    while (true) {
        printDefaultAutoEnter(INFO);

        int startIndex = page * showRowsCount;
        int endIndex = startIndex + showRowsCount - 1;
        if (endIndex >= globalRecordList->size) {
            endIndex = globalRecordList->size - 1;
        }
        maxChoosRowIndex = endIndex - startIndex;
        if (chooseRowIndex > maxChoosRowIndex) chooseRowIndex = maxChoosRowIndex;
        for (int i = startIndex, currentRow = 0; i <= endIndex; i++, currentRow++) {
            if (currentRow == chooseRowIndex) {
                printWaterQualityWhileBkgAutoEnter(getAList(globalRecordList, i));
            } else {
                printWaterQualityAutoEnter(getAList(globalRecordList, i));
            }
        }
        printf(DEL_END_TIPS, page + 1, maxPage + 1);
        enum KeyType key = waitForAnyKey(7, UP, DOWN, LEFT, RIGHT, ESC, BACKSPACE, DEL);

        clearScreen();
        switch (key) {
            case DEL:
                // 删除当前选择的行的逻辑
                removeAListRls(globalRecordList, chooseRowIndex + startIndex);
                // 删除后重新计算最大页数
                maxPage = globalRecordList->size % showRowsCount == 0 ? globalRecordList->size / showRowsCount - 1 : globalRecordList->size / showRowsCount;
                if (page > maxPage) page = maxPage;
                break;
            case BACKSPACE:
            case ESC:
                return;
            case UP:
                chooseRowIndex = chooseRowIndex == 0 ? maxChoosRowIndex : chooseRowIndex - 1;
                break;
            case DOWN:
                chooseRowIndex = chooseRowIndex >= maxChoosRowIndex ? 0 : chooseRowIndex + 1;
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
    globalRecordList = generateRandomWaterQualityData( 1000, mode);
    printDefaultAutoEnter("监测完毕！数据已保存，按下任意键返回");
    getKey();
}
// 查看统计数据
void seeStatistics() {
    if (globalRecordList->size == 0) {
        printDefaultAutoEnter("无统计数据");
        Sleep(3000);
        return;
    }
    // 这里统计计算
    double size = globalRecordList->size;
    struct DataRestriction* normalRestriction;
    struct DataRestriction* seriousRestriction;
    struct DataRestriction* validRestriction;
    if (mode == PENAEUS_VANNAMEI) {
        normalRestriction = penaeusVannameiNormalData;
        seriousRestriction = penaeusVannameiSeriousData;
        validRestriction = penaeusVannameiValidData;
    } else if (mode == MICROPTERUS_SALMOIDES) {
        normalRestriction = micropterusSalmoidesNormalData;
        seriousRestriction = micropterusSalmoidesSeriousData;
        validRestriction = micropterusSalmoidesValidData;
    } else {
        normalRestriction = crassostreaGigasNormalData;
        seriousRestriction = crassostreaGigasSeriousData;
        validRestriction = crassostreaGigasValidData;
    }
    struct WaterQuality* q1 = getAList(globalRecordList, 0);
    double tmpSum = 0, doxygenSum = 0, phSum = 0, ammoniaSum = 0;
    double tmpMax = q1->tmp, doxygenMax = q1->doxygen, phMax = q1->ph, ammoniaMax = q1->ammonia;
    double tmpMin = q1->tmp, doxygenMin = q1->doxygen, phMin = q1->ph, ammoniaMin = q1->ammonia;
    int normalTmpCount = 0, normalDoxygenCount = 0, normalPhCount = 0, normalAmmoniaCount = 0;
    int normalAlertTmpCount = 0, normalAlertDoxygenCount = 0, normalAlertPhCount = 0, normalAlertAmmoniaCount = 0;
    int seriousAlertTmpCount = 0, seriousAlertDoxygenCount = 0, seriousAlertPhCount = 0, seriousAlertAmmoniaCount = 0;
    for (int i = 0; i < globalRecordList->size; i++) {
        struct WaterQuality* q = getAList(globalRecordList, i);
        tmpSum += q->tmp;
        doxygenSum += q->doxygen;
        phSum += q->ph;
        ammoniaSum += q->ammonia;
        tmpMax = q->tmp > tmpMax ? q->tmp : tmpMax;
        doxygenMax = q->doxygen > doxygenMax ? q->doxygen : doxygenMax;
        phMax = q->ph > phMax ? q->ph : phMax;
        ammoniaMax = q->ammonia > ammoniaMax ? q->ammonia : ammoniaMax;
        tmpMin = q->tmp < tmpMin ? q->tmp : tmpMin;
        doxygenMin = q->doxygen < doxygenMin ? q->doxygen : doxygenMin;
        phMin = q->ph < phMin ? q->ph : phMin;
        ammoniaMin = q->ammonia < ammoniaMin ? q->ammonia : ammoniaMin;
        if (q->tmp >= normalRestriction->minTmp && q->tmp <= normalRestriction->maxTmp) {
            normalTmpCount++;
        } else if (q->tmp >= seriousRestriction->minTmp && q->tmp <= seriousRestriction->maxTmp) {
            normalAlertTmpCount++;
        } else if (q->tmp >= validRestriction->minTmp && q->tmp <= validRestriction->maxTmp) {
            seriousAlertTmpCount++;
        }
        if (q->doxygen >= normalRestriction->minDoxygen && q->doxygen <= normalRestriction->maxDoxygen) {
            normalDoxygenCount++;
        } else if (q->doxygen >= seriousRestriction->minDoxygen && q->doxygen <= seriousRestriction->maxDoxygen) {
            normalAlertDoxygenCount++;
        } else if (q->doxygen >= validRestriction->minDoxygen && q->doxygen <= validRestriction->maxDoxygen) {
            seriousAlertDoxygenCount++;
        }
        if (q->ph >= normalRestriction->minPh && q->ph <= normalRestriction->maxPh) {
            normalPhCount++;
        } else if (q->ph >= seriousRestriction->minPh && q->ph <= seriousRestriction->maxPh) {
            normalAlertPhCount++;
        } else if (q->ph >= validRestriction->minPh && q->ph <= validRestriction->maxPh) {
            seriousAlertPhCount++;
        }
        if (q->ammonia >= normalRestriction->minAmmonia && q->ammonia <= normalRestriction->maxAmmonia) {
            normalAmmoniaCount++;
        } else if (q->ammonia >= seriousRestriction->minAmmonia && q->ammonia <= seriousRestriction->maxAmmonia) {
            normalAlertAmmoniaCount++;
        } else if (q->ammonia >= validRestriction->minAmmonia && q->ammonia <= validRestriction->maxAmmonia) {
            seriousAlertAmmoniaCount++;
        }
    }
    //        温度 溶解氧 PH 氨氮
    // 平均值   xx  xx   xx  xx
    // 最大值   xx  xx   xx  xx
    // 最小值   xx  xx   xx  xx
    // 正常(%)
    // 一般告警
    // 严重告警
    double tmpAvg = tmpSum / size, doxygenAvg = doxygenSum / size
        , phAvg = phSum / size, ammoniaAvg = ammoniaSum / size;
    printf("\t\t温度\t溶解氧\tPH\t氨氮\n");
    printf("平均值\t\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n", tmpAvg, doxygenAvg, phAvg, ammoniaAvg);
    printf("最大值\t\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n", tmpMax, doxygenMax, phMax, ammoniaMax);
    printf("最小值\t\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n", tmpMin, doxygenMin, phMin, ammoniaMin);
    printf("正常(%%)\t\t%.2lf%%\t%.2lf%%\t%.2lf%%\t%.2lf%%\n", 100.0 * normalTmpCount / size, 100.0 * normalDoxygenCount / size, 100.0 * normalPhCount / size, 100.0 * normalAmmoniaCount / size);
    printf("一般告警(%%)\t%.2lf%%\t%.2lf%%\t%.2lf%%\t%.2lf%%\n", 100.0 * normalAlertTmpCount / size, 100.0 * normalAlertDoxygenCount / size, 100.0 * normalAlertPhCount / size, 100.0 * normalAlertAmmoniaCount / size);
    printf("严重告警(%%)\t%.2lf%%\t%.2lf%%\t%.2lf%%\t%.2lf%%\n", 100.0 * seriousAlertTmpCount / size, 100.0 * seriousAlertDoxygenCount / size, 100.0 * seriousAlertPhCount / size, 100.0 * seriousAlertAmmoniaCount / size);
    printf("按下Enter键返回");
    waitForRightKey(ENTER);
    clearScreen();
}

// ---------- 用户管理界面（仅管理员）----------
void manageUsers() {
    if (!is_user_logged_in() || !get_current_user()->is_admin) {
        printDefaultAutoEnter("无权限！");
        Sleep(1500);
        return;
    }

    ArrayList users = get_all_users();
    if (!users || users->size == 0) {
        printDefaultAutoEnter("暂无用户！");
        Sleep(1500);
        return;
    }

    int rows = getVisibleRows() - 3;
    int page = 0, row = 0;
    int maxPage = (users->size - 1) / rows;

    while (1) {
        clearScreen();
        printDefaultAutoEnter("=== 用户管理 ===");
        printDefaultAutoEnter("用户名\t\t管理员\t状态");

        int start = page * rows;
        int end = start + rows - 1;
        if (end >= users->size) end = users->size - 1;
        int maxRow = end - start;
        if (row > maxRow) row = maxRow;

        for (int i = start, cur = 0; i <= end; i++, cur++) {
            User* u = (User*)getAList(users, i);
            char line[128];
            sprintf(line, "%s\t\t%s\t%s", u->username,
                    u->is_admin ? "是" : "否",
                    (strcmp(u->username, get_current_user()->username) == 0) ? "当前用户" : "");
            if (cur == row) printfWhileBkgBoolAutoEnter(true, "%s", line);
            else printDefaultAutoEnter("%s", line);
        }
        printf("第 %d/%d 页  方向键切换，Del删除（不能删自己），ESC返回\n", page+1, maxPage+1);

        enum KeyType key = waitForAnyKey(7, UP, DOWN, LEFT, RIGHT, DEL, ESC, BACKSPACE);
        switch (key) {
            case DEL: {
                int idx = start + row;
                User* u = (User*)getAList(users, idx);
                if (strcmp(u->username, get_current_user()->username) == 0) {
                    printDefaultAutoEnter("不能删除当前登录用户！");
                    Sleep(1500);
                    break;
                }
                clearScreen();
                printDefaultAutoEnter("确认删除 %s？(y/n)", u->username);
                if (_getch() == 'y' || _getch() == 'Y') {
                    if (delete_user_by_username(u->username)) {
                        printDefaultAutoEnter("删除成功！");
                        maxPage = (users->size - 1) / rows;
                        if (page > maxPage) page = maxPage;
                        if (row >= (end - start + 1)) row = 0;
                    } else {
                        printDefaultAutoEnter("删除失败！");
                    }
                    Sleep(1500);
                }
                break;
            }
            case ESC: case BACKSPACE: return;
            case UP:   row = (row == 0) ? maxRow : row - 1; break;
            case DOWN: row = (row >= maxRow) ? 0 : row + 1; break;
            case LEFT: if (page > 0) page--; row = 0; break;
            case RIGHT: if (page < maxPage) page++; row = 0; break;
            default: break;
        }
    }
}

// ---------- 主用户菜单 ----------
void userLoopInit() {
    int choice = 1;
    bool isAdmin = is_user_logged_in() && get_current_user()->is_admin;
    int maxChoice = isAdmin ? 7 : 6;

    while (true) {
        printfWhileBkgBoolAutoEnter(choice == 1, "1. 开始监测");
        printfWhileBkgBoolAutoEnter(choice == 2, "2. 查看历史数据");
        printfWhileBkgBoolAutoEnter(choice == 3, "3. 修改历史数据");
        printfWhileBkgBoolAutoEnter(choice == 4, "4. 删除历史数据");
        printfWhileBkgBoolAutoEnter(choice == 5, "5. 查看统计数据");
        if (isAdmin)
            printfWhileBkgBoolAutoEnter(choice == 6, "6. 用户管理");
        printfWhileBkgBoolAutoEnter(choice == maxChoice, isAdmin ? "7. 退出并保存" : "6. 退出并保存");

        enum KeyType key = waitForAnyKey(3, UP, DOWN, ENTER);
        clearScreen();

        switch (key) {
            case UP: choice = (choice == 1) ? maxChoice : choice - 1; break;
            case DOWN: choice = (choice == maxChoice) ? 1 : choice + 1; break;
            case ENTER:
                if (choice == 1) watchInit();
                else if (choice == 2) seeHistoryRecord();
                else if (choice == 3) editHistoryRecord();
                else if (choice == 4) delHistoryRecord();
                else if (choice == 5) seeStatistics();
                else if (choice == 6 && isAdmin) manageUsers();
                else if (choice == maxChoice) {
                    writeWaterQualityRecords(globalRecordList);
                    printDefaultAutoEnter("ByeBye!");
                    Sleep(3000);
                    return;
                }
            clearScreen();
            default: ;
        }
        clearScreen();
    }
}
void initConsole() {
    globalRecordList = readWaterQualityRecords();
    initTerminal();
    init_user_system();         // 初始化用户系统
    if (!user_login_loop()) {   // 登录循环，若选择退出则结束
        printf("已退出系统。\n");
        return;
    }
    clearScreen();
    chooseModeInit();
    userLoopInit();
    exitTerminal();
}