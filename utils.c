#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "models.h"

void save_users() {
    FILE *fp = fopen("users.txt", "w");
    if (!fp) return;
    User *u = users_head;
    while (u) {
        fprintf(fp, "%d,%s,%s,%d\n", u->id, u->username, u->password_enc, u->role);
        u = u->next;
    }
    fclose(fp);
}

void save_clubs() {
    FILE *fp = fopen("clubs.txt", "w");
    if (!fp) return;
    Club *c = clubs_head;
    while (c) {
        fprintf(fp, "%d,%s,%s,%d,%.2f,%d\n", c->id, c->name, c->found_date, c->founder_id, c->balance, c->approved);
        c = c->next;
    }
    fclose(fp);
}

void save_members() {
    FILE *fp = fopen("members.txt", "w");
    if (!fp) return;
    Club *c = clubs_head;
    while (c) {
        Member *m = c->members;
        while (m) {
            fprintf(fp, "%d,%d,%d\n", c->id, m->user_id, m->role_in_club);
            m = m->next;
        }
        c = c->next;
    }
    fclose(fp);
}

void save_transactions() {
    FILE *fp = fopen("transactions.txt", "w");
    if (!fp) return;
    Transaction *t = tx_head;
    while (t) {
        fprintf(fp, "%d,%d,%.2f,%s,%s\n", t->id, t->club_id, t->amount, t->desc, t->time_str);
        t = t->next;
    }
    fclose(fp);
}

void save_activities() {
    FILE *fp = fopen("activities.txt", "w");
    if (!fp) return;
    Activity *a = activities_head;
    while (a) {
        fprintf(fp, "%d,%d,%s,%s,%s,%d\n", a->id, a->club_id, a->name, a->start_time, a->end_time, a->approved);
        a = a->next;
    }
    fclose(fp);
}

void save_participants() {
    FILE *fp = fopen("participants.txt", "w");
    if (!fp) return;
    Activity *a = activities_head;
    while (a) {
        Participant *p = a->participants;
        while (p) {
            fprintf(fp, "%d,%d\n", a->id, p->user_id);
            p = p->next;
        }
        a = a->next;
    }
    fclose(fp);
}

void save_fund_requests() {
    FILE *fp = fopen("fund_requests.txt", "w");
    if (!fp) return;
    FundRequest *r = requests_head;
    while (r) {
        fprintf(fp, "%d,%d,%.2f,%s,%d,%d,%s\n", r->id, r->club_id, r->amount, r->desc, r->requester_id, r->status, r->time_str);
        r = r->next;
    }
    fclose(fp);
}

void save_data() {
    save_users();
    save_clubs();
    save_members();
    save_transactions();
    save_activities();
    save_participants();
    save_fund_requests();
}

void load_users() {
    FILE *fp = fopen("users.txt", "r");
    if (!fp) return;
    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        User *u = malloc(sizeof(User));
        char username[256], password[256];
        sscanf(line, "%d,%[^,],%[^,],%d", &u->id, username, password, (int*)&u->role);
        u->username = malloc(strlen(username) + 1);
        strcpy(u->username, username);
        u->password_enc = malloc(strlen(password) + 1);
        strcpy(u->password_enc, password);
        u->next = users_head;
        users_head = u;
    }
    fclose(fp);
}

void load_clubs() {
    FILE *fp = fopen("clubs.txt", "r");
    if (!fp) return;
    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        Club *c = malloc(sizeof(Club));
        char name[256], found_date[64];
        sscanf(line, "%d,%[^,],%[^,],%d,%lf,%d", &c->id, name, found_date, &c->founder_id, &c->balance, &c->approved);
        c->name = malloc(strlen(name) + 1);
        strcpy(c->name, name);
        c->found_date = malloc(strlen(found_date) + 1);
        strcpy(c->found_date, found_date);
        c->members = NULL;
        c->next = clubs_head;
        clubs_head = c;
    }
    fclose(fp);
}

void load_members() {
    FILE *fp = fopen("members.txt", "r");
    if (!fp) return;
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        int club_id, user_id, role;
        sscanf(line, "%d,%d,%d", &club_id, &user_id, &role);
        Club *c = clubs_head;
        while (c && c->id != club_id) c = c->next;
        if (c) {
            Member *m = malloc(sizeof(Member));
            m->user_id = user_id;
            m->role_in_club = role;
            m->next = c->members;
            c->members = m;
        }
    }
    fclose(fp);
}

void load_transactions() {
    FILE *fp = fopen("transactions.txt", "r");
    if (!fp) return;
    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        Transaction *t = malloc(sizeof(Transaction));
        char desc[512], time_str[64];
        sscanf(line, "%d,%d,%lf,%[^,],%s", &t->id, &t->club_id, &t->amount, desc, time_str);
        t->desc = malloc(strlen(desc) + 1);
        strcpy(t->desc, desc);
        strcpy(t->time_str, time_str);
        t->next = tx_head;
        tx_head = t;
    }
    fclose(fp);
}

void load_activities() {
    FILE *fp = fopen("activities.txt", "r");
    if (!fp) return;
    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        Activity *a = malloc(sizeof(Activity));
        char name[256], start_time[64], end_time[64];
        sscanf(line, "%d,%d,%[^,],%[^,],%[^,],%d", &a->id, &a->club_id, name, start_time, end_time, &a->approved);
        a->name = malloc(strlen(name) + 1);
        strcpy(a->name, name);
        strcpy(a->start_time, start_time);
        strcpy(a->end_time, end_time);
        a->participants = NULL;
        a->next = activities_head;
        activities_head = a;
    }
    fclose(fp);
}

void load_participants() {
    FILE *fp = fopen("participants.txt", "r");
    if (!fp) return;
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        int activity_id, user_id;
        sscanf(line, "%d,%d", &activity_id, &user_id);
        Activity *a = activities_head;
        while (a && a->id != activity_id) a = a->next;
        if (a) {
            Participant *p = malloc(sizeof(Participant));
            p->user_id = user_id;
            p->next = a->participants;
            a->participants = p;
        }
    }
    fclose(fp);
}

void load_fund_requests() {
    FILE *fp = fopen("fund_requests.txt", "r");
    if (!fp) return;
    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        FundRequest *r = malloc(sizeof(FundRequest));
        char desc[512], time_str[64];
        sscanf(line, "%d,%d,%lf,%[^,],%d,%d,%s", &r->id, &r->club_id, &r->amount, desc, &r->requester_id, &r->status, time_str);
        r->desc = malloc(strlen(desc) + 1);
        strcpy(r->desc, desc);
        strcpy(r->time_str, time_str);
        r->next = requests_head;
        requests_head = r;
    }
    fclose(fp);
}

void load_data() {
    load_users();
    load_clubs();
    load_members();
    load_transactions();
    load_activities();
    load_participants();
    load_fund_requests();
}

// Helper function to find club by id
Club* find_club(int club_id) {
    Club *c = clubs_head;
    while (c && c->id != club_id) c = c->next;
    return c;
}