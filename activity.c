#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "activity.h"
#include "utils.h"
#include "models.h"

static int get_next_activity_id() {
    int max_id = 0;
    Activity *a = activities_head;
    while (a) { if (a->id > max_id) max_id = a->id; a = a->next; }
    return max_id + 1;
}

int check_activity_time_conflict(int club_id, const char *start_time, const char *end_time, int *conflict_id) {
    time_t s_new = parse_time_str(start_time);
    time_t e_new = parse_time_str(end_time);
    if (s_new == (time_t)-1 || e_new == (time_t)-1) return -1; // parse error
    Activity *a = activities_head;
    while (a) {
        if (a->club_id == club_id) {
            time_t s_exist = parse_time_str(a->start_time);
            time_t e_exist = parse_time_str(a->end_time);
            if (s_exist == (time_t)-1 || e_exist == (time_t)-1) { a = a->next; continue; }
            if (s_new < e_exist && s_exist < e_new) {
                if (conflict_id) *conflict_id = a->id;
                return 1; // conflict
            }
        }
        a = a->next;
    }
    return 0; // no conflict
}

Activity* create_activity(int club_id, const char *name, const char *start_time, const char *end_time) {
    int conflict_id = -1;
    int check = check_activity_time_conflict(club_id, start_time, end_time, &conflict_id);
    if (check == -1) {
        printf("Invalid time format. Use 'YYYY-MM-DD HH:MM' or similar.\n");
        return NULL;
    }
    if (check == 1) {
        printf("Time conflict with activity id=%d. Creation aborted.\n", conflict_id);
        return NULL;
    }

    Activity *a = malloc(sizeof(Activity));
    if (!a) return NULL;
    a->id = get_next_activity_id();
    a->club_id = club_id;
    a->name = malloc(strlen(name) + 1);
    strcpy(a->name, name);
    strncpy(a->start_time, start_time, sizeof(a->start_time)-1); a->start_time[sizeof(a->start_time)-1]=0;
    strncpy(a->end_time, end_time, sizeof(a->end_time)-1); a->end_time[sizeof(a->end_time)-1]=0;
    a->participants = NULL;
    a->approved = 0;
    a->next = activities_head;
    activities_head = a;
    log_action("Create activity id=%d club=%d name=%s start=%s end=%s", a->id, a->club_id, a->name, a->start_time, a->end_time);
    return a;
}

void list_activities_for_club(int club_id) {
    printf("Activities for club %d:\n", club_id);
    Activity *a = activities_head;
    int found = 0;
    while (a) {
        if (a->club_id == club_id) {
            printf("ID:%d Name:%s Start:%s End:%s Approved:%d\n", a->id, a->name, a->start_time, a->end_time, a->approved);
            found = 1;
        }
        a = a->next;
    }
    if (!found) printf(" No activities found.\n");
}
