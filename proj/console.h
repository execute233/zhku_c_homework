#ifndef CONSOLE_H
#define CONSOLE_H


// 指示选择的养殖类型
enum Mode {
    PENAEUS_VANNAMEI, // 南美对白虾
    MICROPTERUS_SALMOIDES, // 大口黑鲈
    CRASSOSTRA_GIGAS // 南澳牡蛎
};
extern enum Mode mode;
void initConsole();
#endif