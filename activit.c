#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "activity.h"
#include "models.h"

// Helper to get next activity ID
static int get_next_activity_id() {
    int max_id = 0;
    Activity *curr = activities_head;
    while (curr) {
        if (curr->id > max_id) max_id = curr->id;
        curr = curr->next;
    }
    return max_id + 1;
}

void publish_activity(int club_id, const char *name, const char *start_time, const char *end_time) {
    Activity *new_activity = (Activity *)malloc(sizeof(Activity));
    if (!new_activity) {
        printf("Memory allocation failed for new activity.\n");
        return;
    }

    new_activity->id = get_next_activity_id();
    new_activity->club_id = club_id;
    
    new_activity->name = malloc(strlen(name) + 1);
    if (new_activity->name) {
        strcpy(new_activity->name, name);
    }
    
    strncpy(new_activity->start_time, start_time, sizeof(new_activity->start_time) - 1);
    new_activity->start_time[sizeof(new_activity->start_time) - 1] = '\0';
    
    strncpy(new_activity->end_time, end_time, sizeof(new_activity->end_time) - 1);
    new_activity->end_time[sizeof(new_activity->end_time) - 1] = '\0';

    new_activity->participants = NULL;
    new_activity->approved = 1; // Auto-approve for now, or set to 0 if approval needed
    
    new_activity->next = activities_head;
    activities_head = new_activity;

    printf("Activity '%s' published successfully with ID %d.\n", name, new_activity->id);
}

void register_for_activity(int activity_id, int user_id) {
    Activity *curr = activities_head;
    while (curr && curr->id != activity_id) {
        curr = curr->next;
    }

    if (!curr) {
        printf("Activity with ID %d not found.\n", activity_id);
        return;
    }

    // Check if user is already registered
    Participant *p = curr->participants;
    while (p) {
        if (p->user_id == user_id) {
            printf("User %d is already registered for this activity.\n", user_id);
            return;
        }
        p = p->next;
    }

    // Register user
    Participant *new_participant = (Participant *)malloc(sizeof(Participant));
    if (!new_participant) {
        printf("Memory allocation failed for participant.\n");
        return;
    }
    new_participant->user_id = user_id;
    new_participant->next = curr->participants;
    curr->participants = new_participant;

    printf("User %d registered for activity '%s' successfully.\n", user_id, curr->name);
}

void list_activities(int club_id) {
    Activity *curr = activities_head;
    int found = 0;
    
    printf("\n--- Activity List ---\n");
    while (curr) {
        if (club_id == 0 || curr->club_id == club_id) {
            int count = 0;
            Participant *p = curr->participants;
            while (p) { count++; p = p->next; }

            printf("ID: %d | Club ID: %d | Name: %s\n", curr->id, curr->club_id, curr->name);
            printf("   Time: %s - %s\n", curr->start_time, curr->end_time);
            printf("   Participants: %d\n", count);
            printf("---------------------\n");
            found = 1;
        }
        curr = curr->next;
    }

    if (!found) {
        printf("No activities found.\n");
    }
}
