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

void init_user_system(); // 用户管理初始化
bool user_login_loop(); // 登录循环
struct User* get_current_user(); // 获取当前用户
bool is_user_logged_in(); // 判断用户是否已登录
void user_logout(); // 登出
void save_users(); // 保存用户
struct ArrayList* get_all_users(); // 获取所有用户
bool delete_user_by_username(const char* username); // 删除用户
bool change_password(const char* old_pwd, const char* new_pwd); // 修改密码

#endif