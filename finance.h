#ifndef FINANCE_H
#define FINANCE_H

#include "models.h"

// 生成总体财务报表
void generate_financial_report();

// 生成特定俱乐部的财务报表
void generate_club_financial_report(int club_id);

// 生成基金请求报表
void generate_fund_requests_report();

// 验证数据关联
void validate_data_associations();

#endif