#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "finance.h"
#include "models.h"
#include "utils.h"
#include "club.h"

// Helper to get current time string
void get_current_time_str(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

void add_transaction(int club_id, double amount, const char *desc) {
    // Find club to update balance
    Club *c = find_club_by_id(club_id);
    
    if (!c) {
        printf("Club not found!\n");
        return;
    }

    // Create new transaction
    Transaction *t = malloc(sizeof(Transaction));
    if (!t) {
        printf("Memory allocation failed!\n");
        return;
    }
    
    // Generate ID
    int max_id = 0;
    Transaction *curr = tx_head;
    while (curr) {
        if (curr->id > max_id) max_id = curr->id;
        curr = curr->next;
    }
    t->id = max_id + 1;
    
    t->club_id = club_id;
    t->amount = amount;
    t->desc = malloc(strlen(desc) + 1);
    if (t->desc) {
        strcpy(t->desc, desc);
    }
    get_current_time_str(t->time_str, sizeof(t->time_str));
    
    // Add to head
    t->next = tx_head;
    tx_head = t;
    
    // Update club balance
    c->balance += amount;
    printf("Transaction added successfully. New balance: %.2f\n", c->balance);
}

void list_transactions(int club_id) {
    printf("\n--- Transactions for Club ID %d ---\n", club_id);
    Transaction *t = tx_head;
    int found = 0;
    while (t) {
        if (t->club_id == club_id) {
            printf("ID: %d | Amount: %.2f | Date: %s | Desc: %s\n", 
                   t->id, t->amount, t->time_str, t->desc);
            found = 1;
        }
        t = t->next;
    }
    if (!found) {
        printf("No transactions found for this club.\n");
    }
}

void verify_balance(int club_id) {
    Club *c = find_club_by_id(club_id);
    
    if (!c) {
        printf("Club not found!\n");
        return;
    }
    
    double calculated_balance = 0.0;
    Transaction *t = tx_head;
    while (t) {
        if (t->club_id == club_id) {
            calculated_balance += t->amount;
        }
        t = t->next;
    }
    
    printf("\n--- Balance Verification for Club: %s ---\n", c->name);
    printf("Stored Balance: %.2f\n", c->balance);
    printf("Calculated Balance from Transactions: %.2f\n", calculated_balance);
    
    if (fabs(c->balance - calculated_balance) < 0.001) {
        printf("Status: MATCHED\n");
    } else {
        printf("Status: MISMATCH (Difference: %.2f)\n", c->balance - calculated_balance);
    }
}

// 生成总体财务报表
void generate_financial_report() {
    printf("\n=== 总体财务报表 ===\n");
    Club *c = clubs_head;
    double total_income = 0, total_expense = 0;
    while (c) {
        double club_income = 0, club_expense = 0;
        Transaction *t = tx_head;
        while (t) {
            if (t->club_id == c->id) {
                if (t->amount > 0) club_income += t->amount;
                else club_expense += t->amount;
            }
            t = t->next;
        }
        printf("俱乐部 %d (%s): 收入 %.2f, 支出 %.2f, 余额 %.2f\n",
               c->id, c->name, club_income, club_expense, c->balance);
        total_income += club_income;
        total_expense += club_expense;
        c = c->next;
    }
    printf("总计: 收入 %.2f, 支出 %.2f\n", total_income, total_expense);
}

// 生成特定俱乐部的财务报表
void generate_club_financial_report(int club_id) {
    Club *c = find_club_by_id(club_id);
    if (!c) {
        printf("俱乐部 ID %d 不存在。\n", club_id);
        return;
    }
    printf("\n=== 俱乐部 %d (%s) 财务报表 ===\n", c->id, c->name);
    double income = 0, expense = 0;
    Transaction *t = tx_head;
    while (t) {
        if (t->club_id == club_id) {
            if (t->amount > 0) income += t->amount;
            else expense += t->amount;
            printf("交易 ID %d: %.2f (%s) - %s\n", t->id, t->amount, t->desc, t->time_str);
        }
        t = t->next;
    }
    printf("总收入: %.2f, 总支出: %.2f, 当前余额: %.2f\n", income, expense, c->balance);
}

// 生成基金请求报表
void generate_fund_requests_report() {
    printf("\n=== 基金请求报表 ===\n");
    FundRequest *r = requests_head;
    while (r) {
        char *status_str;
        if (r->status == 0) status_str = "待处理";
        else if (r->status == 1) status_str = "已批准";
        else status_str = "已拒绝";
        printf("请求 ID %d: 俱乐部 %d, 金额 %.2f, 描述 %s, 请求者 %d, 状态 %s, 时间 %s\n",
               r->id, r->club_id, r->amount, r->desc, r->requester_id, status_str, r->time_str);
        r = r->next;
    }
}

// 验证数据关联
void validate_data_associations() {
    printf("\n=== 数据关联验证 ===\n");
    int errors = 0;
    Transaction *t = tx_head;
    while (t) {
        if (!find_club_by_id(t->club_id)) {
            printf("错误: 交易 ID %d 关联不存在的俱乐部 %d\n", t->id, t->club_id);
            errors++;
        }
        t = t->next;
    }
    FundRequest *r = requests_head;
    while (r) {
        if (!find_club_by_id(r->club_id)) {
            printf("错误: 基金请求 ID %d 关联不存在的俱乐部 %d\n", r->id, r->club_id);
            errors++;
        }
        r = r->next;
    }
    if (errors == 0) {
        printf("所有数据关联正确。\n");
    } else {
        printf("发现 %d 个关联错误。\n", errors);
    }
}
