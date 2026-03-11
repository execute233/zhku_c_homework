#include "console.h"
#include <stdio.h>
#include <stdarg.h>
#include <conio.h>
#include <windows.h>
#define ESC "\033"
#define CLEAR_SCREEN ESC "[2J"
#define MOVE_CURSOR(y, x) ESC "[" #y ";" #x "H"
#define COLOR_RED ESC "[31m"
#define COLOR_GREEN ESC "[32m"
#define COLOR_BOLD ESC "[1m"
#define RESET ESC "[0m"
#define HIDE_CURSOR ESC "[?25l"
#define SHOW_CURSOR ESC "[?25h"
#define WHITE_BG    ESC "[47m"
#define BLACK_FG    ESC "[30m"
#define BOLD        ESC "[1m"
#define STYLE_INVERTED WHITE_BG BLACK_FG
#define CLEAR_AND_HOME "\033[2J\033[H"
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
    UNKOWN
};


void setTerminal() {
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
char getKey() {
    return _getch();
}
enum KeyType waitForKey() {
    while (true) {
        int part1 = _getch();
        if (part1 == 0 || part1 == 224) {
            switch (_getch()) {
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
    printf(CLEAR_SCREEN);
    printf(CLEAR_AND_HOME);
}
enum Mode mode = PENAEUS_VANNAMEI;
void printfWhiteAutoEnter(char* format, ...) {
    va_list args;
    va_start(args, format);
    printf(STYLE_INVERTED);
    printf(format, args);
    va_end(args);
    printf(RESET);
    printf("\n");
}
void printfWhileBoolAutoEnter(bool condition, char* format, ...) {
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
void chooseModeInit() {
    int choose = 1;
    while (true) {
        printfWhileBoolAutoEnter(choose == 1, "南美白对虾");
        printfWhileBoolAutoEnter(choose == 2, "大口黑鲈");
        printfWhileBoolAutoEnter(choose == 3, "南奥牡蛎");
        auto key = waitForAnyKey(3, UP, DOWN, ENTER);
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
void initConsole() {
    setTerminal();
    chooseModeInit();

    printf(SHOW_CURSOR);
}