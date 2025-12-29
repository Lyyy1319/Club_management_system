#ifndef FINANCE_H
#define FINANCE_H

void add_transaction(int club_id, double amount, const char *desc);
void list_transactions(int club_id);
void verify_balance(int club_id);

#endif
