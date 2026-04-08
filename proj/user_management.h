#ifndef USER_MANAGEMENT_H
#define USER_MANAGEMENT_H

#include <stdbool.h>
#include "../lib/array_list.h"
// 用户结构体
struct User {
    char username[50]; // 用户名
    char password[50]; // 密码
    bool is_admin; // 是否为管理员
};

void initUserSystem(); // 用户管理初始化
bool userLoginLoop(); // 登录循环
struct User* getCurrentUser(); // 获取当前用户
bool isUserLggedIn(); // 判断用户是否已登录
void userLogout(); // 登出
void saveUsers(); // 保存用户
struct ArrayList* getAllUsers(); // 获取所有用户
bool deleteUserByUsername(const char* username); // 删除用户
bool changePassword(const char* old_pwd, const char* new_pwd); // 修改密码

#endif