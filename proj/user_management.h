#ifndef USER_MANAGEMENT_H
#define USER_MANAGEMENT_H

#include <stdbool.h>
#include "../lib/array_list.h"

typedef struct {
    char username[50];
    char password[50];
    bool is_admin;
} User;

void init_user_system();
bool user_login_loop();
User* get_current_user();
bool is_user_logged_in();
void user_logout();
void save_users();
ArrayList get_all_users();
bool delete_user_by_username(const char* username);

//新
bool change_password(const char* old_pwd, const char* new_pwd);

#endif