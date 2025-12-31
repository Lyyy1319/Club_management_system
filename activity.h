#ifndef ACTIVITY_H
#define ACTIVITY_H

#include "models.h"

// Publish a new activity for a club
void publish_activity(int club_id, const char *name, const char *start_time, const char *end_time);

// Register a user for an activity
void register_for_activity(int activity_id, int user_id);

// List all activities (optionally filtered by club_id if > 0)
void list_activities(int club_id);

#endif
