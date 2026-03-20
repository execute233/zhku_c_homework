/**
 * @file user_management.c
 * @brief 用户管理模块实现，包括用户数据的文件存储、登录、注册、删除等操作。
 *
 * 用户数据保存在 users.txt 文件中，格式为：用户名,密码,管理员标志(0/1)
 * 程序首次运行时自动创建默认管理员账户 (admin/123456)。
 */

#include "user_management.h"
#include "console.h"   // 提供控制台界面函数：clearScreen, printDefaultAutoEnter, waitForAnyKey 等
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>      // _getch()
#include <windows.h>    // Sleep()

#define USER_FILE "users.txt"   /**< 存储用户数据的文件名 */

static User current_user;       /**< 当前登录的用户信息（登录成功后填充） */
static bool logged_in = false;  /**< 登录状态标志 */
static ArrayList user_list = NULL; /**< 存储所有用户的动态数组（元素类型为 User*） */

/**
 * @brief 从控制台读取密码，不回显且以星号掩码显示，支持退格删除。
 *
 * 函数使用 _getch() 逐个读取字符，不回显；每输入一个字符打印一个星号；
 * 退格键（\b 或 ASCII 8）会删除最后一个输入的字符（如果存在）。
 *
 * @param pwd      输出缓冲区，用于存储输入的密码字符串（以 '\0' 结尾）。
 * @param max_len  缓冲区的最大长度，包括结尾的 '\0'。函数最多读取 max_len-1 个字符。
 *
 * @note 该函数仅适用于 Windows 平台（依赖 _getch 和控制台退格处理）。
 */
static void get_password(char* pwd, int max_len) {
    int i = 0;
    char ch;
    while ((ch = _getch()) != '\r' && i < max_len - 1) {
        if (ch == '\b' || ch == 8) {          // 退格键
            if (i > 0) {
                i--;
                printf("\b \b");               // 擦除上一个星号
            }
        } else {
            pwd[i++] = ch;
            printf("*");                        // 显示掩码
        }
    }
    pwd[i] = '\0';
    printf("\n");
}

/**
 * @brief 从文件加载用户数据到 user_list。
 *
 * 如果文件不存在，则创建一个默认管理员账户（admin/123456）并保存到文件。
 * 文件存在时逐行解析，每行格式：用户名,密码,管理员标志(0/1)
 * 解析成功后将 User 结构添加到 user_list；解析失败则跳过该行。
 *
 * @note 使用 sscanf 时限制了字段最大长度为 49，防止缓冲区溢出。
 */
static void load_users() {
    user_list = createAList(10);
    if (!user_list) {
        return;
    }

    FILE* f = fopen(USER_FILE, "r");
    if (!f) {
        // 文件不存在，创建默认管理员账户
        User* admin = (User*)malloc(sizeof(User));
        if (admin) {
            strcpy(admin->username, "admin");
            strcpy(admin->password, "123456");
            admin->is_admin = true;
            addAList(user_list, admin);
            save_users();                       // 立即写入文件
        }
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        // 去除行尾换行符（兼容 Windows 和 Unix）
        size_t len = strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
            line[--len] = '\0';
        }

        User* u = (User*)malloc(sizeof(User));
        if (!u) continue;  // 内存不足，跳过该行

        int admin_flag = 0;  // 临时变量，用于接收整数
        // 使用宽度限制：最多读取 49 个字符到 username 和 password
        if (sscanf(line, "%49[^,],%49[^,],%d",
                   u->username, u->password, &admin_flag) == 3) {
            // 将整数转换为 bool（非零视为 true）
            u->is_admin = (admin_flag != 0);
            addAList(user_list, u);
        } else {
            free(u);  // 解析失败，释放内存
        }
    }
    fclose(f);
}

/**
 * @brief 将当前用户列表保存到文件。
 *
 * 以写入模式打开文件，遍历 user_list 中的每个用户，
 * 按 "用户名,密码,管理员标志\n" 格式写入。
 */
void save_users() {
    FILE* f = fopen(USER_FILE, "w");
    if (!f) return;
    for (int i = 0; i < user_list->size; i++) {
        User* u = (User*)getAList(user_list, i);
        fprintf(f, "%s,%s,%d\n", u->username, u->password, u->is_admin);
    }
    fclose(f);
}

/**
 * @brief 根据用户名查找用户。
 *
 * @param username 要查找的用户名
 * @return User* 指向找到的用户结构体的指针，若未找到返回 NULL。
 */
static User* find_user(const char* username) {
    for (int i = 0; i < user_list->size; i++) {
        User* u = (User*)getAList(user_list, i);
        if (strcmp(u->username, username) == 0) return u;
    }
    return NULL;
}

/**
 * @brief 初始化用户系统，加载用户数据并设置初始状态。
 *
 * 应在程序启动时调用，负责加载用户文件并重置登录状态。
 */
void init_user_system() {
    load_users();
    logged_in = false;
    memset(&current_user, 0, sizeof(User));
}

/**
 * @brief 用户登录/注册主循环。
 *
 * 显示登录/注册/退出菜单，根据用户选择执行相应操作。
 * 登录成功时返回 true，用户选择退出时返回 false。
 *
 * @return true  用户成功登录
 * @return false 用户选择退出系统
 */
bool user_login_loop() {
    int choice = 1;   // 当前选中的菜单项 (1=登录, 2=注册, 3=退出)
    while (1) {
        clearScreen();
        printDefaultAutoEnter("=== 水产养殖系统 - 用户登录 ===");
        printfWhileBkgBoolAutoEnter(choice == 1, "[1] 登录");
        printfWhileBkgBoolAutoEnter(choice == 2, "[2] 注册");
        printfWhileBkgBoolAutoEnter(choice == 3, "[3] 退出");

        enum KeyType key = waitForAnyKey(3, UP, DOWN, ENTER);
        switch (key) {
            case UP: choice = (choice == 1) ? 3 : choice - 1; break;
            case DOWN: choice = (choice == 3) ? 1 : choice + 1; break;
            case ENTER:
                if (choice == 1) { // 登录
                    clearScreen();
                    printDefaultAutoEnter("用户名:");
                    char name[50];
                    scanf("%49s", name);
                    while(getchar()!='\n') {}
                    // 清空输入缓冲
                    printDefaultAutoEnter("密码:");
                    char pwd[50];
                    get_password(pwd, 50);

                    User* u = find_user(name);
                    if (u && strcmp(u->password, pwd) == 0) {
                        current_user = *u;   // 保存当前登录用户信息
                        logged_in = true;
                        printDefaultAutoEnter("登录成功！");
                        Sleep(1000);
                        return true;
                    } else {
                        printDefaultAutoEnter("用户名或密码错误！");
                        Sleep(1500);
                    }
                } else if (choice == 2) { // 注册
                    clearScreen();
                    printDefaultAutoEnter("新用户名:");
                    char name[50];
                    scanf("%49s", name); while(getchar()!='\n');
                    if (find_user(name)) {
                        printDefaultAutoEnter("用户已存在！");
                        Sleep(1500);
                        break;
                    }
                    printDefaultAutoEnter("新密码:");
                    char pwd[50];
                    get_password(pwd, 50);

                    User* nu = (User*)malloc(sizeof(User));
                    strcpy(nu->username, name);
                    strcpy(nu->password, pwd);
                    nu->is_admin = false;   // 注册用户默认为普通用户
                    addAList(user_list, nu);
                    save_users();            // 立即保存到文件
                    printDefaultAutoEnter("注册成功！请登录");
                    Sleep(1500);
                    choice = 1;              // 回到登录选项
                    clearScreen();
                    continue;
                } else { // 退出
                    return false;
                }
                break;
            default: break;
        }
    }
}

/**
 * @brief 获取当前登录用户的指针。
 *
 * @return User* 若已登录返回指向 current_user 的指针，否则返回 NULL。
 */
User* get_current_user() {
    return logged_in ? &current_user : NULL;
}

/**
 * @brief 检查当前是否有用户已登录。
 *
 * @return true  用户已登录
 * @return false 未登录
 */
bool is_user_logged_in() {
    return logged_in;
}

/**
 * @brief 注销当前用户，清空登录状态。
 */
void user_logout() {
    logged_in = false;
    memset(&current_user, 0, sizeof(User));
}

/**
 * @brief 获取所有用户的列表（用于显示或管理）。
 *
 * @return ArrayList 包含所有 User* 的列表。
 */
ArrayList get_all_users() {
    return user_list;
}

/**
 * @brief 根据用户名删除指定用户（仅管理员可调用）。
 *
 * 删除前会检查调用者是否为管理员，且不能删除当前登录用户。
 * 成功删除后更新文件。
 *
 * @param username 要删除的用户名
 * @return true  删除成功
 * @return false 删除失败（权限不足、用户不存在或试图删除自己）
 */
bool delete_user_by_username(const char* username) {
    if (!logged_in || !current_user.is_admin) return false;   // 仅管理员可操作
    if (strcmp(current_user.username, username) == 0) return false; // 不能删除自己

    for (int i = 0; i < user_list->size; i++) {
        User* u = (User*)getAList(user_list, i);
        if (strcmp(u->username, username) == 0) {
            free(u);                           // 释放用户结构体内存
            removeAList(user_list, i);          // 从列表中移除指针
            save_users();                       // 更新文件
            return true;
        }
    }
    return false;   // 未找到用户
}

/**
 * @brief 修改当前登录用户的密码
 * @param old_pwd 旧密码
 * @param new_pwd 新密码
 * @return true 修改成功；false 修改失败（未登录或旧密码错误）
 */
bool change_password(const char* old_pwd, const char* new_pwd) {
    // 1. 检查是否已登录
    if (!logged_in) {
        return false;
    }

    // 2. 验证旧密码
    if (strcmp(current_user.password, old_pwd) != 0) {
        return false;
    }

    // 3. 更新当前用户对象的密码
    strcpy(current_user.password, new_pwd);

    // 4. 同步更新 user_list 中对应条目的密码
    for (int i = 0; i < user_list->size; i++) {
        User* u = (User*)getAList(user_list, i);
        if (strcmp(u->username, current_user.username) == 0) {
            strcpy(u->password, new_pwd);
            break;
        }
    }

    // 5. 保存到文件
    save_users();

    return true;
}