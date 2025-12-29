#include <stdio.h>
#include <stdlib.h>
#include "finance.h"
#include "models.h"
#include "utils.h"

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
    Club *c = find_club(club_id);
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
        if (!find_club(t->club_id)) {
            printf("错误: 交易 ID %d 关联不存在的俱乐部 %d\n", t->id, t->club_id);
            errors++;
        }
        t = t->next;
    }
    FundRequest *r = requests_head;
    while (r) {
        if (!find_club(r->club_id)) {
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