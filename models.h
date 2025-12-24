#ifndef MODELS_H
#define MODELS_H

#include <stdbool.h>

typedef enum {ROLE_MEMBER=0, ROLE_PRESIDENT=1, ROLE_ADMIN=2, ROLE_OFFICER=3} Role;

typedef struct User{
    int id;
    char *username;
    char *password_enc; // stored encrypted
    Role role;
    struct User *next;
} User;

typedef struct Member{
    int user_id;
    Role role_in_club; // officer/president/member
    struct Member *next;
} Member;

typedef struct Club{
    int id;
    char *name;
    char *found_date;
    int founder_id;
    Member *members;
    double balance;
    int approved; // 0 pending, 1 approved
    struct Club *next;
} Club;

typedef struct Transaction{
    int id;
    int club_id;
    double amount; // positive income, negative expense
    char *desc;
    char time_str[64];
    struct Transaction *next;
} Transaction;

// Globals: heads
extern User *users_head;
extern Club *clubs_head;
extern Transaction *tx_head;

typedef struct Participant{
    int user_id;
    struct Participant *next;
} Participant;

typedef struct Activity{
    int id;
    int club_id;
    char *name;
    char start_time[64]; // simple ISO-like string
    char end_time[64];
    Participant *participants;
    int approved; // 0 pending, 1 approved
    struct Activity *next;
} Activity;

extern Activity *activities_head;

typedef struct FundRequest{
    int id;
    int club_id;
    double amount;
    char *desc;
    int requester_id;
    int status; // 0 pending,1 approved,2 denied
    char time_str[64];
    struct FundRequest *next;
} FundRequest;

extern FundRequest *requests_head;

#endif