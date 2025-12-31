#ifndef ACTIVITY_H
#define ACTIVITY_H

#include "models.h"

// Time conflict / creation helpers
// Check time conflict within the same club. Returns 1 if conflict found and sets *conflict_id, 0 otherwise, -1 on parse error.
int check_activity_time_conflict(int club_id, const char *start_time, const char *end_time, int *conflict_id);

// Create a new activity for a club. Returns pointer to Activity on success, NULL on failure or conflict.
Activity* create_activity(int club_id, const char *name, const char *start_time, const char *end_time);

// Publish a new activity for a club (wrapper)
void publish_activity(int club_id, const char *name, const char *start_time, const char *end_time);

// Register a user for an activity
void register_for_activity(int activity_id, int user_id);

// List activities: if club_id > 0, list for that club; otherwise list all
void list_activities(int club_id);

// Backward-compatible simpler listing
void list_activities_for_club(int club_id);

#endif
