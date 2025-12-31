#ifndef ACTIVITY_H
#define ACTIVITY_H

#include "models.h"

// Check time conflict within the same club. Returns 1 if conflict found and sets *conflict_id, 0 otherwise.
int check_activity_time_conflict(int club_id, const char *start_time, const char *end_time, int *conflict_id);

// Create a new activity for a club. Returns pointer to Activity on success, NULL on failure or conflict.
Activity* create_activity(int club_id, const char *name, const char *start_time, const char *end_time);

// List activities for a club (simple print)
void list_activities_for_club(int club_id);

#endif

