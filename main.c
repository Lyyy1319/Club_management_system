#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "models.h"
#include "auth.h"
#include "club.h"
#include "finance.h"
#include "activity.h"
#include "utils.h"

// 兼容性：strdup在某些编译器中可能不可用
#if defined(__GNUC__) && !defined(_GNU_SOURCE)
char* strdup(const char* s) {
    size_t len = strlen(s) + 1;
    char* new_str = malloc(len);
    if (new_str) memcpy(new_str, s, len);
    return new_str;
}
#endif
#ifdef _MSC_VER
#define strdup _strdup
#endif

// 全局变量
User* users_head = NULL;
Club* clubs_head = NULL;
Transaction* tx_head = NULL;
Activity* activities_head = NULL;
FundRequest* requests_head = NULL;

// 当前登录用户
User* current_user = NULL;
int current_club_id = -1;  // 当前用户所属社团ID（如果是社长或成员）

// 清屏函数
void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// 暂停
void pause_screen() {
    printf("\n按回车键继续...");
    getchar();
    while (getchar() != '\n');
}

// 获取用户输入（安全版本）
void get_input(char* buffer, int size) {
    if (fgets(buffer, size, stdin)) {
        buffer[strcspn(buffer, "\n")] = 0;
    }
}

// 简单异或加密
char* xor_encrypt(const char* password) {
    const char key = 0x5A;  // 异或密钥
    int len = strlen(password);
    char* encrypted = malloc(len * 2 + 1);  // 每个字符用2位十六进制表示
    if (!encrypted) return NULL;
    
    encrypted[0] = '\0';
    for (int i = 0; i < len; i++) {
        char temp[3];
        sprintf(temp, "%02X", (unsigned char)(password[i] ^ key));
        strcat(encrypted, temp);
    }
    return encrypted;
}

// 异或解密（用于验证）
char* xor_decrypt(const char* encrypted) {
    const char key = 0x5A;
    int len = strlen(encrypted) / 2;
    char* decrypted = malloc(len + 1);
    if (!decrypted) return NULL;
    
    for (int i = 0; i < len; i++) {
        unsigned int val;
        sscanf(encrypted + i * 2, "%2X", &val);
        decrypted[i] = (char)(val ^ key);
    }
    decrypted[len] = '\0';
    return decrypted;
}

// 查找用户所属的社团（作为社长）
Club* find_club_as_president(int user_id) {
    Club* c = clubs_head;
    while (c) {
        if (c->founder_id == user_id && c->approved == 1) {
            return c;
        }
        c = c->next;
    }
    return NULL;
}

// 查找用户所属的社团（作为成员）
Club* find_club_as_member(int user_id) {
    Club* c = clubs_head;
    while (c) {
        if (c->approved == 1) {
            Member* m = c->members;
            while (m) {
                if (m->user_id == user_id) {
                    return c;
                }
                m = m->next;
            }
        }
        c = c->next;
    }
    return NULL;
}

// 获取用户在社团中的角色
Role get_user_role_in_club(int user_id, Club* club) {
    if (!club) return ROLE_MEMBER;
    if (club->founder_id == user_id) return ROLE_PRESIDENT;
    Member* m = club->members;
    while (m) {
        if (m->user_id == user_id) {
            return m->role_in_club;
        }
        m = m->next;
    }
    return ROLE_MEMBER;
}

// ===================== 登录注册系统 =====================

void show_login_menu() {
    printf("\n===== 社团管理系统 =====\n");
    printf("1. 登录\n");
    printf("2. 注册\n");
    printf("0. 退出\n");
    printf("请选择: ");
}

int do_register() {
    char username[100], password[100], password2[100];
    int role_choice;
    
    printf("\n===== 用户注册 =====\n");
    printf("用户名: ");
    get_input(username, sizeof(username));
    
    if (find_user_by_username(username)) {
        printf("错误: 用户名已存在！\n");
        return 0;
    }
    
    printf("密码: ");
    get_input(password, sizeof(password));
    printf("确认密码: ");
    get_input(password2, sizeof(password2));
    
    if (strcmp(password, password2) != 0) {
        printf("错误: 两次输入的密码不一致！\n");
        return 0;
    }
    
    printf("选择角色:\n");
    printf("  0. 普通成员\n");
    printf("  1. 社长（需要创建或申请社团）\n");
    printf("  2. 管理员（社联）\n");
    printf("请选择: ");
    scanf("%d", &role_choice);
    while (getchar() != '\n');
    
    Role role;
    switch (role_choice) {
        case 0: role = ROLE_MEMBER; break;
        case 1: role = ROLE_PRESIDENT; break;
        case 2: role = ROLE_ADMIN; break;
        default: role = ROLE_MEMBER; break;
    }
    
    // 使用异或加密密码
    char* encrypted = xor_encrypt(password);
    if (!encrypted) {
        printf("错误: 密码加密失败！\n");
        return 0;
    }
    
    User* new_user = malloc(sizeof(User));
    if (!new_user) {
        free(encrypted);
        return 0;
    }
    
    static int next_id = 1;
    // 找到最大ID
    User* u = users_head;
    while (u) {
        if (u->id >= next_id) next_id = u->id + 1;
        u = u->next;
    }
    
    new_user->id = next_id++;
    new_user->username = strdup(username);
    new_user->password_enc = encrypted;
    new_user->role = role;
    new_user->next = users_head;
    users_head = new_user;
    
    save_data();
    log_action("用户注册: %s (ID=%d, 角色=%d)", username, new_user->id, role);
    printf("注册成功！您的用户ID是: %d\n", new_user->id);
    return 1;
}

User* do_login() {
    char username[100], password[100];
    
    printf("\n===== 用户登录 =====\n");
    printf("用户名: ");
    get_input(username, sizeof(username));
    printf("密码: ");
    get_input(password, sizeof(password));
    
    User* user = find_user_by_username(username);
    if (!user) {
        printf("错误: 用户名不存在！\n");
        return NULL;
    }
    
    // 使用异或加密验证密码
    char* encrypted_input = xor_encrypt(password);
    if (!encrypted_input) {
        printf("错误: 密码处理失败！\n");
        return NULL;
    }
    
    if (strcmp(user->password_enc, encrypted_input) == 0) {
        free(encrypted_input);
        log_action("用户登录: %s (ID=%d)", username, user->id);
        printf("登录成功！欢迎, %s\n", user->username);
        
        const char* role_names[] = {"普通成员", "社长", "管理员（社联）", "干事"};
        printf("您的角色: %s\n", role_names[user->role]);
        
        return user;
    } else {
        free(encrypted_input);
        printf("错误: 密码错误！\n");
        return NULL;
    }
}

// ===================== 管理员（社联）菜单 =====================

void admin_approve_club() {
    printf("\n===== 待审批社团列表 =====\n");
    Club* c = clubs_head;
    int found = 0;
    while (c) {
        if (c->approved == 0) {
            printf("ID: %d | 名称: %s | 成立日期: %s | 申请人ID: %d\n",
                   c->id, c->name, c->found_date, c->founder_id);
            found = 1;
        }
        c = c->next;
    }
    
    if (!found) {
        printf("没有待审批的社团。\n");
        return;
    }
    
    printf("\n输入要审批的社团ID（0取消）: ");
    int club_id;
    scanf("%d", &club_id);
    while (getchar() != '\n');
    
    if (club_id == 0) return;
    
    c = find_club_by_id(club_id);
    if (!c || c->approved != 0) {
        printf("社团不存在或已审批。\n");
        return;
    }
    
    printf("1. 批准  2. 拒绝\n请选择: ");
    int choice;
    scanf("%d", &choice);
    while (getchar() != '\n');
    
    if (choice == 1) {
        c->approved = 1;
        printf("社团 '%s' 已批准！\n", c->name);
        log_action("管理员批准社团: %s (ID=%d)", c->name, c->id);
    } else if (choice == 2) {
        delete_club(club_id);
        printf("社团申请已拒绝并删除。\n");
        log_action("管理员拒绝社团申请: ID=%d", club_id);
    }
    save_data();
}

void admin_dissolve_club() {
    printf("\n===== 注销社团 =====\n");
    printf("已批准的社团列表:\n");
    Club* c = clubs_head;
    while (c) {
        if (c->approved == 1) {
            // 计算成员数
            int member_count = 0;
            Member* m = c->members;
            while (m) { member_count++; m = m->next; }
            printf("ID: %d | 名称: %s | 成员数: %d | 余额: %.2f\n",
                   c->id, c->name, member_count, c->balance);
        }
        c = c->next;
    }
    
    printf("\n输入要注销的社团ID（0取消）: ");
    int club_id;
    scanf("%d", &club_id);
    while (getchar() != '\n');
    
    if (club_id == 0) return;
    
    c = find_club_by_id(club_id);
    if (!c) {
        printf("社团不存在。\n");
        return;
    }
    
    printf("确认要注销社团 '%s' 吗？(y/n): ", c->name);
    char confirm;
    scanf("%c", &confirm);
    while (getchar() != '\n');
    
    if (confirm == 'y' || confirm == 'Y') {
        log_action("管理员注销社团: %s (ID=%d)", c->name, c->id);
        delete_club(club_id);
        printf("社团已注销。\n");
        save_data();
    } else {
        printf("取消注销。\n");
    }
}

void admin_approve_activity() {
    printf("\n===== 待审批活动列表 =====\n");
    Activity* a = activities_head;
    int found = 0;
    while (a) {
        if (a->approved == 0) {
            Club* c = find_club_by_id(a->club_id);
            printf("ID: %d | 社团: %s | 活动名: %s\n", 
                   a->id, c ? c->name : "未知", a->name);
            printf("   时间: %s ~ %s\n", a->start_time, a->end_time);
            found = 1;
        }
        a = a->next;
    }
    
    if (!found) {
        printf("没有待审批的活动。\n");
        return;
    }
    
    printf("\n输入要审批的活动ID（0取消）: ");
    int act_id;
    scanf("%d", &act_id);
    while (getchar() != '\n');
    
    if (act_id == 0) return;
    
    a = activities_head;
    while (a && a->id != act_id) a = a->next;
    
    if (!a || a->approved != 0) {
        printf("活动不存在或已审批。\n");
        return;
    }
    
    printf("1. 批准  2. 拒绝\n请选择: ");
    int choice;
    scanf("%d", &choice);
    while (getchar() != '\n');
    
    if (choice == 1) {
        a->approved = 1;
        printf("活动 '%s' 已批准！\n", a->name);
        log_action("管理员批准活动: %s (ID=%d)", a->name, a->id);
    } else {
        // 删除活动
        printf("活动已拒绝。\n");
        log_action("管理员拒绝活动: ID=%d", act_id);
    }
    save_data();
}

void admin_approve_fund_request() {
    printf("\n===== 待审批经费申请 =====\n");
    FundRequest* r = requests_head;
    int found = 0;
    while (r) {
        if (r->status == 0) {
            Club* c = find_club_by_id(r->club_id);
            printf("ID: %d | 社团: %s | 金额: %.2f | 描述: %s\n",
                   r->id, c ? c->name : "未知", r->amount, r->desc);
            printf("   申请人ID: %d | 时间: %s\n", r->requester_id, r->time_str);
            found = 1;
        }
        r = r->next;
    }
    
    if (!found) {
        printf("没有待审批的经费申请。\n");
        return;
    }
    
    printf("\n输入要审批的申请ID（0取消）: ");
    int req_id;
    scanf("%d", &req_id);
    while (getchar() != '\n');
    
    if (req_id == 0) return;
    
    r = requests_head;
    while (r && r->id != req_id) r = r->next;
    
    if (!r || r->status != 0) {
        printf("申请不存在或已处理。\n");
        return;
    }
    
    printf("1. 批准  2. 拒绝\n请选择: ");
    int choice;
    scanf("%d", &choice);
    while (getchar() != '\n');
    
    if (choice == 1) {
        r->status = 1;
        // 添加收入记录
        add_transaction(r->club_id, r->amount, r->desc);
        printf("经费申请已批准，%.2f元已拨付。\n", r->amount);
        log_action("管理员批准经费申请: ID=%d 金额=%.2f", r->id, r->amount);
    } else {
        r->status = 2;
        printf("经费申请已拒绝。\n");
        log_action("管理员拒绝经费申请: ID=%d", r->id);
    }
    save_data();
}

void admin_view_all_clubs() {
    printf("\n===== 所有社团列表 =====\n");
    Club* c = clubs_head;
    while (c) {
        int member_count = 0;
        Member* m = c->members;
        while (m) { member_count++; m = m->next; }
        
        printf("ID: %d | 名称: %s | 状态: %s\n",
               c->id, c->name, c->approved ? "已批准" : "待审批");
        printf("   成立日期: %s | 负责人ID: %d | 成员数: %d | 余额: %.2f\n",
               c->found_date, c->founder_id, member_count, c->balance);
        printf("----------------------------------------\n");
        c = c->next;
    }
}

void show_admin_menu() {
    int choice;
    do {
        printf("\n===== 管理员菜单（社联）=====\n");
        printf("--- 审批管理 ---\n");
        printf("1. 审批新社团注册\n");
        printf("2. 注销社团\n");
        printf("3. 审批活动申请\n");
        printf("4. 审批经费申请\n");
        printf("--- 查看信息 ---\n");
        printf("5. 查看所有社团\n");
        printf("6. 查看所有活动\n");
        printf("7. 生成总体财务报表\n");
        printf("8. 查看操作日志\n");
        printf("--- 数据管理 ---\n");
        printf("9. 搜索社团（模糊匹配）\n");
        printf("10. 社团排序\n");
        printf("11. 创建备份\n");
        printf("12. 恢复备份\n");
        printf("0. 退出登录\n");
        printf("请选择: ");
        scanf("%d", &choice);
        while (getchar() != '\n');
        
        switch (choice) {
            case 1: admin_approve_club(); break;
            case 2: admin_dissolve_club(); break;
            case 3: admin_approve_activity(); break;
            case 4: admin_approve_fund_request(); break;
            case 5: admin_view_all_clubs(); break;
            case 6: list_activities(0); break;
            case 7: generate_financial_report(); break;
            case 8: {
                printf("\n===== 操作日志 =====\n");
                FILE* fp = fopen("operation.log", "r");
                if (fp) {
                    char line[256];
                    while (fgets(line, sizeof(line), fp)) {
                        printf("%s", line);
                    }
                    fclose(fp);
                } else {
                    printf("无法读取日志文件。\n");
                }
                break;
            }
            case 9: {
                char keyword[100];
                printf("输入搜索关键词: ");
                get_input(keyword, sizeof(keyword));
                search_clubs_fuzzy(keyword);
                break;
            }
            case 10: {
                printf("排序方式:\n");
                printf("1. 按ID\n");
                printf("2. 按名称\n");
                printf("3. 按余额（降序）\n");
                printf("4. 按成员数（降序）\n");
                printf("请选择: ");
                int sort_choice;
                scanf("%d", &sort_choice);
                while (getchar() != '\n');
                sort_clubs(sort_choice);
                admin_view_all_clubs();
                break;
            }
            case 11: {
                if (create_backup() == 0) {
                    printf("备份创建成功！\n");
                } else {
                    printf("备份创建失败。\n");
                }
                break;
            }
            case 12: {
                char backup_dir[256];
                printf("输入备份目录路径（如 backups/backup_20231201_120000）: ");
                get_input(backup_dir, sizeof(backup_dir));
                if (restore_backup(backup_dir) == 0) {
                    load_data();
                    printf("备份恢复成功！\n");
                } else {
                    printf("备份恢复失败。\n");
                }
                break;
            }
            case 0:
                printf("退出登录。\n");
                break;
            default:
                printf("无效选择。\n");
        }
    } while (choice != 0);
}

// ===================== 社长菜单 =====================

void president_create_club() {
    char name[100], found_date[20];
    
    printf("\n===== 申请创建社团 =====\n");
    printf("社团名称: ");
    get_input(name, sizeof(name));
    printf("成立日期 (YYYY-MM-DD): ");
    get_input(found_date, sizeof(found_date));
    
    Club* new_club = create_club(name, found_date, current_user->id);
    if (new_club) {
        printf("社团 '%s' 申请已提交，等待管理员审批。\n", name);
        printf("社团ID: %d\n", new_club->id);
        save_data();
    } else {
        printf("创建失败。\n");
    }
}

void president_add_member() {
    if (current_club_id < 0) {
        printf("您还没有已批准的社团。\n");
        return;
    }
    
    printf("\n===== 添加社团成员 =====\n");
    printf("输入要添加的用户ID: ");
    int user_id;
    scanf("%d", &user_id);
    while (getchar() != '\n');
    
    // 查找用户是否存在
    User* u = users_head;
    while (u && u->id != user_id) u = u->next;
    if (!u) {
        printf("用户不存在。\n");
        return;
    }
    
    printf("选择角色:\n");
    printf("0. 普通成员\n");
    printf("3. 干事\n");
    printf("请选择: ");
    int role;
    scanf("%d", &role);
    while (getchar() != '\n');
    
    add_member_to_club(current_club_id, user_id, (Role)role);
    printf("成员 %s 已添加到社团。\n", u->username);
    save_data();
}

void president_remove_member() {
    if (current_club_id < 0) {
        printf("您还没有已批准的社团。\n");
        return;
    }
    
    Club* c = find_club_by_id(current_club_id);
    if (!c) return;
    
    printf("\n===== 移除社团成员 =====\n");
    printf("当前成员列表:\n");
    Member* m = c->members;
    while (m) {
        User* u = users_head;
        while (u && u->id != m->user_id) u = u->next;
        const char* role_names[] = {"成员", "社长", "管理员", "干事"};
        printf("ID: %d | 用户名: %s | 角色: %s\n",
               m->user_id, u ? u->username : "未知", role_names[m->role_in_club]);
        m = m->next;
    }
    
    printf("\n输入要移除的用户ID（0取消）: ");
    int user_id;
    scanf("%d", &user_id);
    while (getchar() != '\n');
    
    if (user_id != 0) {
        remove_member_from_club(current_club_id, user_id);
        printf("成员已移除。\n");
        save_data();
    }
}

void president_change_member_role() {
    if (current_club_id < 0) {
        printf("您还没有已批准的社团。\n");
        return;
    }
    
    Club* c = find_club_by_id(current_club_id);
    if (!c) return;
    
    printf("\n===== 修改成员角色 =====\n");
    printf("当前成员列表:\n");
    Member* m = c->members;
    while (m) {
        User* u = users_head;
        while (u && u->id != m->user_id) u = u->next;
        const char* role_names[] = {"成员", "社长", "管理员", "干事"};
        printf("ID: %d | 用户名: %s | 当前角色: %s\n",
               m->user_id, u ? u->username : "未知", role_names[m->role_in_club]);
        m = m->next;
    }
    
    printf("\n输入用户ID（0取消）: ");
    int user_id;
    scanf("%d", &user_id);
    while (getchar() != '\n');
    
    if (user_id == 0) return;
    
    printf("选择新角色:\n");
    printf("0. 普通成员\n");
    printf("3. 干事\n");
    printf("请选择: ");
    int role;
    scanf("%d", &role);
    while (getchar() != '\n');
    
    change_member_role(current_club_id, user_id, (Role)role);
    save_data();
}

void president_publish_activity() {
    if (current_club_id < 0) {
        printf("您还没有已批准的社团。\n");
        return;
    }
    
    char name[100], start_time[64], end_time[64];
    
    printf("\n===== 发布活动 =====\n");
    printf("活动名称: ");
    get_input(name, sizeof(name));
    printf("开始时间 (YYYY-MM-DD HH:MM): ");
    get_input(start_time, sizeof(start_time));
    printf("结束时间 (YYYY-MM-DD HH:MM): ");
    get_input(end_time, sizeof(end_time));
    
    Activity* a = create_activity(current_club_id, name, start_time, end_time);
    if (a) {
        printf("活动 '%s' 已创建，等待管理员审批。\n", name);
        save_data();
    }
}

void president_request_fund() {
    if (current_club_id < 0) {
        printf("您还没有已批准的社团。\n");
        return;
    }
    
    double amount;
    char desc[256];
    
    printf("\n===== 申请经费 =====\n");
    printf("申请金额: ");
    scanf("%lf", &amount);
    while (getchar() != '\n');
    printf("用途说明: ");
    get_input(desc, sizeof(desc));
    
    // 创建经费申请
    FundRequest* r = malloc(sizeof(FundRequest));
    if (!r) return;
    
    static int next_req_id = 1;
    FundRequest* fr = requests_head;
    while (fr) {
        if (fr->id >= next_req_id) next_req_id = fr->id + 1;
        fr = fr->next;
    }
    
    r->id = next_req_id++;
    r->club_id = current_club_id;
    r->amount = amount;
    r->desc = strdup(desc);
    r->requester_id = current_user->id;
    r->status = 0;  // 待审批
    
    time_t now = time(NULL);
    struct tm* lt = localtime(&now);
    strftime(r->time_str, sizeof(r->time_str), "%Y-%m-%d %H:%M:%S", lt);
    
    r->next = requests_head;
    requests_head = r;
    
    printf("经费申请已提交，等待管理员审批。\n");
    log_action("社长申请经费: 社团ID=%d 金额=%.2f", current_club_id, amount);
    save_data();
}

void president_add_transaction() {
    if (current_club_id < 0) {
        printf("您还没有已批准的社团。\n");
        return;
    }
    
    double amount;
    char desc[256];
    int type;
    
    printf("\n===== 添加交易记录 =====\n");
    printf("类型: 1. 收入（会费/赞助）  2. 支出（活动报销）\n");
    printf("请选择: ");
    scanf("%d", &type);
    while (getchar() != '\n');
    
    printf("金额: ");
    scanf("%lf", &amount);
    while (getchar() != '\n');
    
    if (type == 2) {
        // 检查余额是否足够
        Club* c = find_club_by_id(current_club_id);
        if (c && c->balance < amount) {
            printf("错误: 余额不足！当前余额: %.2f\n", c->balance);
            return;
        }
        amount = -amount;  // 支出为负数
    }
    
    printf("描述: ");
    get_input(desc, sizeof(desc));
    
    add_transaction(current_club_id, amount, desc);
    save_data();
}

void show_president_menu() {
    // 查找该社长的社团
    Club* my_club = find_club_as_president(current_user->id);
    if (my_club) {
        current_club_id = my_club->id;
        printf("您管理的社团: %s (ID: %d)\n", my_club->name, my_club->id);
    } else {
        current_club_id = -1;
    }
    
    int choice;
    do {
        printf("\n===== 社长菜单 =====\n");
        printf("--- 社团管理 ---\n");
        printf("1. 申请创建社团\n");
        printf("2. 查看我的社团信息\n");
        printf("--- 成员管理 ---\n");
        printf("3. 添加社团成员\n");
        printf("4. 移除社团成员\n");
        printf("5. 修改成员角色（干事/成员）\n");
        printf("--- 活动管理 ---\n");
        printf("6. 发布活动\n");
        printf("7. 查看活动列表\n");
        printf("--- 财务管理 ---\n");
        printf("8. 申请经费\n");
        printf("9. 添加交易记录（收入/支出）\n");
        printf("10. 查看交易记录\n");
        printf("11. 查看社团财务报表\n");
        printf("12. 验证社团余额\n");
        printf("0. 退出登录\n");
        printf("请选择: ");
        scanf("%d", &choice);
        while (getchar() != '\n');
        
        switch (choice) {
            case 1: president_create_club(); 
                    // 刷新社团信息
                    my_club = find_club_as_president(current_user->id);
                    if (my_club) current_club_id = my_club->id;
                    break;
            case 2:
                if (current_club_id >= 0) {
                    Club* c = find_club_by_id(current_club_id);
                    if (c) {
                        int member_count = 0;
                        Member* m = c->members;
                        while (m) { member_count++; m = m->next; }
                        printf("\n===== 社团信息 =====\n");
                        printf("ID: %d\n", c->id);
                        printf("名称: %s\n", c->name);
                        printf("成立日期: %s\n", c->found_date);
                        printf("状态: %s\n", c->approved ? "已批准" : "待审批");
                        printf("成员数: %d\n", member_count);
                        printf("余额: %.2f\n", c->balance);
                    }
                } else {
                    printf("您还没有已批准的社团。\n");
                }
                break;
            case 3: president_add_member(); break;
            case 4: president_remove_member(); break;
            case 5: president_change_member_role(); break;
            case 6: president_publish_activity(); break;
            case 7: 
                if (current_club_id >= 0) {
                    list_activities(current_club_id);
                } else {
                    printf("您还没有已批准的社团。\n");
                }
                break;
            case 8: president_request_fund(); break;
            case 9: president_add_transaction(); break;
            case 10:
                if (current_club_id >= 0) {
                    list_transactions(current_club_id);
                } else {
                    printf("您还没有已批准的社团。\n");
                }
                break;
            case 11:
                if (current_club_id >= 0) {
                    generate_club_financial_report(current_club_id);
                } else {
                    printf("您还没有已批准的社团。\n");
                }
                break;
            case 12:
                if (current_club_id >= 0) {
                    verify_balance(current_club_id);
                } else {
                    printf("您还没有已批准的社团。\n");
                }
                break;
            case 0:
                printf("退出登录。\n");
                break;
            default:
                printf("无效选择。\n");
        }
    } while (choice != 0);
}

// ===================== 普通成员菜单 =====================

void member_view_clubs() {
    printf("\n===== 社团列表 =====\n");
    Club* c = clubs_head;
    while (c) {
        if (c->approved == 1) {
            int member_count = 0;
            Member* m = c->members;
            while (m) { member_count++; m = m->next; }
            printf("ID: %d | 名称: %s | 成员数: %d\n", c->id, c->name, member_count);
        }
        c = c->next;
    }
}

void member_join_club() {
    printf("\n===== 加入社团 =====\n");
    member_view_clubs();
    
    printf("\n输入要加入的社团ID（0取消）: ");
    int club_id;
    scanf("%d", &club_id);
    while (getchar() != '\n');
    
    if (club_id == 0) return;
    
    Club* c = find_club_by_id(club_id);
    if (!c || c->approved != 1) {
        printf("社团不存在或未批准。\n");
        return;
    }
    
    // 检查是否已经是成员
    Member* m = find_member_in_club(c, current_user->id);
    if (m) {
        printf("您已经是该社团的成员。\n");
        return;
    }
    
    add_member_to_club(club_id, current_user->id, ROLE_MEMBER);
    printf("您已成功加入社团 '%s'！\n", c->name);
    save_data();
}

void member_leave_club() {
    printf("\n===== 退出社团 =====\n");
    
    // 查找用户所在的社团
    printf("您加入的社团:\n");
    Club* c = clubs_head;
    int found = 0;
    while (c) {
        if (c->approved == 1) {
            Member* m = find_member_in_club(c, current_user->id);
            if (m) {
                printf("ID: %d | 名称: %s\n", c->id, c->name);
                found = 1;
            }
        }
        c = c->next;
    }
    
    if (!found) {
        printf("您没有加入任何社团。\n");
        return;
    }
    
    printf("\n输入要退出的社团ID（0取消）: ");
    int club_id;
    scanf("%d", &club_id);
    while (getchar() != '\n');
    
    if (club_id == 0) return;
    
    c = find_club_by_id(club_id);
    if (!c) {
        printf("社团不存在。\n");
        return;
    }
    
    remove_member_from_club(club_id, current_user->id);
    printf("您已退出社团 '%s'。\n", c->name);
    save_data();
}

// 检查用户活动时间冲突
int check_user_activity_conflict(int user_id, Activity* target_activity) {
    time_t target_start = parse_time_str(target_activity->start_time);
    time_t target_end = parse_time_str(target_activity->end_time);
    
    Activity* a = activities_head;
    while (a) {
        if (a->id != target_activity->id && a->approved == 1) {
            // 检查用户是否已报名该活动
            Participant* p = a->participants;
            while (p) {
                if (p->user_id == user_id) {
                    // 检查时间冲突
                    time_t exist_start = parse_time_str(a->start_time);
                    time_t exist_end = parse_time_str(a->end_time);
                    
                    if (target_start < exist_end && exist_start < target_end) {
                        printf("警告: 与活动 '%s' 时间冲突！\n", a->name);
                        printf("冲突时间: %s ~ %s\n", a->start_time, a->end_time);
                        return 1;
                    }
                    break;
                }
                p = p->next;
            }
        }
        a = a->next;
    }
    return 0;
}

void member_register_activity() {
    printf("\n===== 报名参加活动 =====\n");
    
    // 显示已批准的活动
    Activity* a = activities_head;
    int found = 0;
    while (a) {
        if (a->approved == 1) {
            Club* c = find_club_by_id(a->club_id);
            int count = 0;
            Participant* p = a->participants;
            while (p) { count++; p = p->next; }
            printf("ID: %d | 社团: %s | 活动: %s\n",
                   a->id, c ? c->name : "未知", a->name);
            printf("   时间: %s ~ %s | 已报名: %d人\n",
                   a->start_time, a->end_time, count);
            found = 1;
        }
        a = a->next;
    }
    
    if (!found) {
        printf("目前没有可报名的活动。\n");
        return;
    }
    
    printf("\n输入要报名的活动ID（0取消）: ");
    int act_id;
    scanf("%d", &act_id);
    while (getchar() != '\n');
    
    if (act_id == 0) return;
    
    a = activities_head;
    while (a && a->id != act_id) a = a->next;
    
    if (!a || a->approved != 1) {
        printf("活动不存在或未批准。\n");
        return;
    }
    
    // 检查是否已报名
    Participant* p = a->participants;
    while (p) {
        if (p->user_id == current_user->id) {
            printf("您已经报名了该活动。\n");
            return;
        }
        p = p->next;
    }
    
    // 检查时间冲突
    if (check_user_activity_conflict(current_user->id, a)) {
        printf("是否仍要报名？(y/n): ");
        char confirm;
        scanf("%c", &confirm);
        while (getchar() != '\n');
        if (confirm != 'y' && confirm != 'Y') {
            printf("取消报名。\n");
            return;
        }
    }
    
    register_for_activity(act_id, current_user->id);
    save_data();
}

void member_view_my_status() {
    printf("\n===== 个人状态 =====\n");
    printf("用户ID: %d\n", current_user->id);
    printf("用户名: %s\n", current_user->username);
    const char* role_names[] = {"普通成员", "社长", "管理员（社联）", "干事"};
    printf("系统角色: %s\n", role_names[current_user->role]);
    
    printf("\n--- 加入的社团 ---\n");
    Club* c = clubs_head;
    int found = 0;
    while (c) {
        if (c->approved == 1) {
            Member* m = find_member_in_club(c, current_user->id);
            if (m) {
                printf("社团: %s | 角色: %s\n", c->name, role_names[m->role_in_club]);
                found = 1;
            }
        }
        c = c->next;
    }
    if (!found) printf("您还没有加入任何社团。\n");
    
    printf("\n--- 报名的活动 ---\n");
    Activity* a = activities_head;
    found = 0;
    while (a) {
        Participant* p = a->participants;
        while (p) {
            if (p->user_id == current_user->id) {
                Club* ac = find_club_by_id(a->club_id);
                printf("活动: %s | 社团: %s\n", a->name, ac ? ac->name : "未知");
                printf("   时间: %s ~ %s\n", a->start_time, a->end_time);
                found = 1;
                break;
            }
            p = p->next;
        }
        a = a->next;
    }
    if (!found) printf("您还没有报名任何活动。\n");
}

void show_member_menu() {
    int choice;
    do {
        printf("\n===== 普通成员菜单 =====\n");
        printf("--- 社团 ---\n");
        printf("1. 查看社团列表\n");
        printf("2. 搜索社团（模糊匹配）\n");
        printf("3. 加入社团\n");
        printf("4. 退出社团\n");
        printf("--- 活动 ---\n");
        printf("5. 查看活动列表\n");
        printf("6. 报名参加活动\n");
        printf("--- 个人 ---\n");
        printf("7. 查询个人状态\n");
        printf("0. 退出登录\n");
        printf("请选择: ");
        scanf("%d", &choice);
        while (getchar() != '\n');
        
        switch (choice) {
            case 1: member_view_clubs(); break;
            case 2: {
                char keyword[100];
                printf("输入搜索关键词: ");
                get_input(keyword, sizeof(keyword));
                search_clubs_fuzzy(keyword);
                break;
            }
            case 3: member_join_club(); break;
            case 4: member_leave_club(); break;
            case 5: list_activities(0); break;
            case 6: member_register_activity(); break;
            case 7: member_view_my_status(); break;
            case 0:
                printf("退出登录。\n");
                break;
            default:
                printf("无效选择。\n");
        }
    } while (choice != 0);
}

// ===================== 主程序 =====================

int main() {
#ifdef _WIN32
    // 设置控制台代码页为UTF-8
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    // 同时设置字体（如果支持）
    system("chcp 65001 > nul");
#endif
    setlocale(LC_ALL, "zh_CN.UTF-8");
    
    // 加载数据
    load_data();
    log_action("系统启动");
    
    printf("欢迎使用社团管理系统！\n");
    printf("数据已从文件加载。\n");
    
    int choice;
    do {
        show_login_menu();
        scanf("%d", &choice);
        while (getchar() != '\n');
        
        switch (choice) {
            case 1: {
                current_user = do_login();
                if (current_user) {
                    switch (current_user->role) {
                        case ROLE_ADMIN:
                            show_admin_menu();
                            break;
                        case ROLE_PRESIDENT:
                            show_president_menu();
                            break;
                        case ROLE_MEMBER:
                        case ROLE_OFFICER:
                        default:
                            show_member_menu();
                            break;
                    }
                    current_user = NULL;
                    current_club_id = -1;
                }
                break;
            }
            case 2:
                do_register();
                break;
            case 0:
                printf("正在保存数据...\n");
                save_data();
                log_action("系统退出");
                printf("数据已保存，再见！\n");
                break;
            default:
                printf("无效选择。\n");
        }
    } while (choice != 0);
    
    return 0;
}
