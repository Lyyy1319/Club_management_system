#ifndef FINANCE_H
#define FINANCE_H

#include "models.h"

// Transaction management
void add_transaction(int club_id, double amount, const char *desc);
void list_transactions(int club_id);
void verify_balance(int club_id);

// Reporting
void generate_financial_report();
void generate_club_financial_report(int club_id);
void generate_fund_requests_report();
void validate_data_associations();

#endif
