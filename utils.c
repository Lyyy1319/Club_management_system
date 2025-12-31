#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#define mkdir(path,mode) _mkdir(path)
#endif
#include "models.h"
#include "utils.h"

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

// ----- Operation log implementation -----
void log_action(const char *fmt, ...) {
    FILE *fp = fopen("operation.log", "a");
    if (!fp) return;
    time_t t = time(NULL);
    struct tm tm;
    struct tm *lt = localtime(&t);
    if (lt) tm = *lt; else memset(&tm,0,sizeof(tm));
    char timestr[64];
    strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", &tm);
    fprintf(fp, "[%s] ", timestr);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(fp, fmt, ap);
    va_end(ap);
    fprintf(fp, "\n");
    fclose(fp);
}

// Helper: copy file (binary-safe)
static int copy_file(const char *src, const char *dst) {
    FILE *fsrc = fopen(src, "rb");
    if (!fsrc) return -1;
    FILE *fdst = fopen(dst, "wb");
    if (!fdst) { fclose(fsrc); return -2; }
    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), fsrc)) > 0) {
        if (fwrite(buf, 1, n, fdst) != n) { fclose(fsrc); fclose(fdst); return -3; }
    }
    fclose(fsrc);
    fclose(fdst);
    return 0;
}

// Create backups/<timestamp>/ and copy data files
int create_backup() {
    time_t t = time(NULL);
    struct tm tm;
    struct tm *lt = localtime(&t);
    if (lt) tm = *lt; else memset(&tm,0,sizeof(tm));
    char dirname[256];
    strftime(dirname, sizeof(dirname), "backups/backup_%Y%m%d_%H%M%S", &tm);
    // create backups dir if needed
    mkdir("backups", 0755);
    if (mkdir(dirname, 0755) != 0) {
        // On some platforms mkdir returns -1 if exists; we ignore that
    }
    const char *files[] = {"users.txt","clubs.txt","members.txt","transactions.txt","activities.txt","participants.txt","fund_requests.txt","operation.log"};
    int i;
    for (i = 0; i < (int)(sizeof(files)/sizeof(files[0])); i++) {
        char dst[512];
        snprintf(dst, sizeof(dst), "%s/%s", dirname, files[i]);
        // ignore errors for missing files, but try to copy
        copy_file(files[i], dst);
    }
    log_action("Created backup: %s", dirname);
    return 0;
}

// Restore files from given backup directory
int restore_backup(const char *backup_dir) {
    if (!backup_dir) return -1;
    const char *files[] = {"users.txt","clubs.txt","members.txt","transactions.txt","activities.txt","participants.txt","fund_requests.txt"};
    int i;
    for (i = 0; i < (int)(sizeof(files)/sizeof(files[0])); i++) {
        char src[512];
        snprintf(src, sizeof(src), "%s/%s", backup_dir, files[i]);
        // copy back to working dir
        if (copy_file(src, files[i]) != 0) {
            // continue even if some files fail
        }
    }
    log_action("Restored backup from: %s", backup_dir);
    return 0;
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

// ---------- Fuzzy matching (Levenshtein) ----------
int levenshtein_distance(const char *s1, const char *s2) {
    if (!s1) s1 = "";
    if (!s2) s2 = "";
    size_t len1 = strlen(s1), len2 = strlen(s2);
    if (len1 == 0) return (int)len2;
    if (len2 == 0) return (int)len1;

    int *prev = malloc((len2 + 1) * sizeof(int));
    int *cur = malloc((len2 + 1) * sizeof(int));
    if (!prev || !cur) {
        if (prev) free(prev);
        if (cur) free(cur);
        return INT_MAX/2;
    }

    for (size_t j = 0; j <= len2; j++) prev[j] = (int)j;

    for (size_t i = 1; i <= len1; i++) {
        cur[0] = (int)i;
        for (size_t j = 1; j <= len2; j++) {
            int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
            int a = prev[j] + 1;
            int b = cur[j-1] + 1;
            int c = prev[j-1] + cost;
            int v = a < b ? a : b;
            if (c < v) v = c;
            cur[j] = v;
        }
        for (size_t j = 0; j <= len2; j++) prev[j] = cur[j];
    }

    int res = cur[len2];
    free(prev);
    free(cur);
    return res;
}

int fuzzy_match(const char *s, const char *pattern, int max_distance) {
    if (!s || !pattern) return 0;
    // simple case-insensitive substring check first
    char *hs = malloc(strlen(s) + 1);
    char *hp = malloc(strlen(pattern) + 1);
    if (!hs || !hp) { if (hs) free(hs); if (hp) free(hp); return 0; }
    for (size_t i = 0; i <= strlen(s); i++) hs[i] = tolower((unsigned char)s[i]);
    for (size_t i = 0; i <= strlen(pattern); i++) hp[i] = tolower((unsigned char)pattern[i]);
    int contains = (strstr(hs, hp) != NULL);
    free(hs);
    free(hp);
    if (contains) return 1;

    int dist = levenshtein_distance(s, pattern);
    return dist <= max_distance;
}

// ---------- Time parsing for activities ----------
time_t parse_time_str(const char *time_str) {
    if (!time_str) return (time_t)-1;
    struct tm tm;
    memset(&tm, 0, sizeof(tm));
    int year=0, month=0, day=0, hour=0, min=0, sec=0;
    // support formats: "YYYY-MM-DD HH:MM[:SS]" or "YYYY-MM-DD_HH:MM[:SS]"
    if (sscanf(time_str, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &min, &sec) >= 5) {
        // ok
    } else if (sscanf(time_str, "%d-%d-%d_%d:%d:%d", &year, &month, &day, &hour, &min, &sec) >= 5) {
        // ok
    } else if (sscanf(time_str, "%d/%d/%d %d:%d:%d", &year, &month, &day, &hour, &min, &sec) >= 5) {
        // ok
    } else if (sscanf(time_str, "%d-%d-%d %d:%d", &year, &month, &day, &hour, &min) >= 4) {
        sec = 0;
    } else {
        return (time_t)-1; // unrecognized
    }

    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = min;
    tm.tm_sec = sec;
    tm.tm_isdst = -1;
    return mktime(&tm);
}