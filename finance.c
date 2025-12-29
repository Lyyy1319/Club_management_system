#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "finance.h"
#include "models.h"

// Helper to get current time string
void get_current_time_str(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

void add_transaction(int club_id, double amount, const char *desc) {
    // Find club to update balance
    Club *c = clubs_head;
    while (c && c->id != club_id) {
        c = c->next;
    }
    
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
    Club *c = clubs_head;
    while (c && c->id != club_id) {
        c = c->next;
    }
    
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
