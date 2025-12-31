#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "club.h"
#include "models.h"
#include "utils.h"

// Helper function to generate next club ID
int get_next_club_id() {
    int max_id = 0;
    Club* c = clubs_head;
    while (c) {
        if (c->id > max_id) max_id = c->id;
        c = c->next;
    }
    return max_id + 1;
}

// Create a new club
Club* create_club(char* name, char* found_date, int founder_id) {
    Club* new_club = malloc(sizeof(Club));
    if (!new_club) return NULL;

    new_club->id = get_next_club_id();
    new_club->name = malloc(strlen(name) + 1);
    strcpy(new_club->name, name);
    new_club->found_date = malloc(strlen(found_date) + 1);
    strcpy(new_club->found_date, found_date);
    new_club->founder_id = founder_id;
    new_club->members = NULL;
    new_club->balance = 0.0;
    new_club->approved = 0; // pending
    new_club->next = NULL;

    insert_club_at_head(new_club);
    log_action("Create club id=%d name=%s founder=%d", new_club->id, new_club->name, founder_id);
    return new_club;
}

// Find club by ID
Club* find_club_by_id(int id) {
    Club* c = clubs_head;
    while (c) {
        if (c->id == id) return c;
        c = c->next;
    }
    return NULL;
}

// Update club
void update_club(int id, char* name, char* found_date, int founder_id, double balance, int approved) {
    Club* c = find_club_by_id(id);
    if (!c) return;

    free(c->name);
    c->name = malloc(strlen(name) + 1);
    strcpy(c->name, name);

    free(c->found_date);
    c->found_date = malloc(strlen(found_date) + 1);
    strcpy(c->found_date, found_date);

    c->founder_id = founder_id;
    c->balance = balance;
    c->approved = approved;
}

// Delete club
void delete_club(int id) {
    remove_club_from_list(id);
    log_action("Delete club id=%d", id);
}

// Add member to club
void add_member_to_club(int club_id, int user_id, Role role) {
    Club* c = find_club_by_id(club_id);
    if (!c) return;

    Member* m = find_member_in_club(c, user_id);
    if (m) {
        m->role_in_club = role; // update role if already member
        return;
    }

    Member* new_member = malloc(sizeof(Member));
    if (!new_member) return;

    new_member->user_id = user_id;
    new_member->role_in_club = role;
    new_member->next = NULL;

    insert_member_at_head(c, new_member);
    log_action("Add member user=%d to club=%d role=%d", user_id, club_id, role);
}

// Remove member from club
void remove_member_from_club(int club_id, int user_id) {
    Club* c = find_club_by_id(club_id);
    if (!c) return;

    remove_member_from_club_list(c, user_id);
    log_action("Remove member user=%d from club=%d", user_id, club_id);
}

// Change member role
void change_member_role(int club_id, int user_id, Role new_role) {
    Club* c = find_club_by_id(club_id);
    if (!c) {
        printf("Club not found.\n");
        return;
    }
    Member* m = find_member_in_club(c, user_id);
    if (!m) {
        printf("Member not found in this club.\n");
        return;
    }
    m->role_in_club = new_role;
    printf("Member role changed successfully.\n");
    log_action("Change member role user=%d in club=%d to=%d", user_id, club_id, new_role);
}

// Insert club at head of list
void insert_club_at_head(Club* new_club) {
    new_club->next = clubs_head;
    clubs_head = new_club;
}

// Remove club from list
void remove_club_from_list(int id) {
    if (!clubs_head) return;

    if (clubs_head->id == id) {
        Club* temp = clubs_head;
        clubs_head = clubs_head->next;
        free(temp->name);
        free(temp->found_date);
        // Free members list
        Member* m = temp->members;
        while (m) {
            Member* next = m->next;
            free(m);
            m = next;
        }
        free(temp);
        return;
    }

    Club* prev = clubs_head;
    Club* curr = clubs_head->next;
    while (curr) {
        if (curr->id == id) {
            prev->next = curr->next;
            free(curr->name);
            free(curr->found_date);
            // Free members
            Member* m = curr->members;
            while (m) {
                Member* next = m->next;
                free(m);
                m = next;
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

// Find member in club
Member* find_member_in_club(Club* club, int user_id) {
    Member* m = club->members;
    while (m) {
        if (m->user_id == user_id) return m;
        m = m->next;
    }
    return NULL;
}

// Insert member at head of club's members list
void insert_member_at_head(Club* club, Member* new_member) {
    new_member->next = club->members;
    club->members = new_member;
}

// Remove member from club's members list
void remove_member_from_club_list(Club* club, int user_id) {
    if (!club->members) return;

    if (club->members->user_id == user_id) {
        Member* temp = club->members;
        club->members = club->members->next;
        free(temp);
        return;
    }

    Member* prev = club->members;
    Member* curr = club->members->next;
    while (curr) {
        if (curr->user_id == user_id) {
            prev->next = curr->next;
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}
// Helper for case-insensitive substring search
int str_contains_ignore_case(const char* haystack, const char* needle) {
    if (!needle || !*needle) return 1;
    if (!haystack || !*haystack) return 0;
    
    // Create copies to avoid modifying originals
    char *h = malloc(strlen(haystack) + 1);
    char *n = malloc(strlen(needle) + 1);
    if (!h || !n) {
        if(h) free(h);
        if(n) free(n);
        return 0;
    }
    strcpy(h, haystack);
    strcpy(n, needle);
    
    for(int i=0; h[i]; i++) h[i] = tolower((unsigned char)h[i]);
    for(int i=0; n[i]; i++) n[i] = tolower((unsigned char)n[i]);
    
    int result = (strstr(h, n) != NULL);
    
    free(h);
    free(n);
    return result;
}

void search_clubs_fuzzy(const char* keyword) {
    printf("\n--- Search Results for '%s' ---\n", keyword);
    Club* c = clubs_head;
    int found = 0;
    int max_distance = 2;
    if (keyword && strlen(keyword) > 6) max_distance = (int)(strlen(keyword) / 3);
    while (c) {
        if (str_contains_ignore_case(c->name, keyword)) {
            printf("ID: %d | Name: %s | Balance: %.2f | Approved: %d\n",
                   c->id, c->name, c->balance, c->approved);
            found = 1;
        } else {
            if (fuzzy_match(c->name, keyword, max_distance)) {
                printf("ID: %d | Name: %s | Balance: %.2f | Approved: %d (fuzzy)\n",
                       c->id, c->name, c->balance, c->approved);
                found = 1;
            }
        }
        c = c->next;
    }
    if (!found) {
        printf("No clubs found matching '%s'.\n", keyword);
    }
    printf("-------------------------------\n");
}

void swap_club_data(Club *a, Club *b) {
    int temp_id = a->id; a->id = b->id; b->id = temp_id;
    
    char *temp_name = a->name; a->name = b->name; b->name = temp_name;
    
    char *temp_date = a->found_date; a->found_date = b->found_date; b->found_date = temp_date;
    
    int temp_founder = a->founder_id; a->founder_id = b->founder_id; b->founder_id = temp_founder;
    
    Member *temp_members = a->members; a->members = b->members; b->members = temp_members;
    
    double temp_bal = a->balance; a->balance = b->balance; b->balance = temp_bal;
    
    int temp_app = a->approved; a->approved = b->approved; b->approved = temp_app;
}

void sort_clubs(int criteria) {
    if (!clubs_head || !clubs_head->next) return;
    
    int swapped;
    Club *ptr1;
    Club *lptr = NULL;
    
    do {
        swapped = 0;
        ptr1 = clubs_head;
        
        while (ptr1->next != lptr) {
            int should_swap = 0;
            switch (criteria) {
                case 1: // ID
                    if (ptr1->id > ptr1->next->id) should_swap = 1;
                    break;
                case 2: // Name
                    if (strcmp(ptr1->name, ptr1->next->name) > 0) should_swap = 1;
                    break;
                case 3: // Balance (Descending)
                    if (ptr1->balance < ptr1->next->balance) should_swap = 1;
                    break;
            }
            
            if (should_swap) {
                swap_club_data(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
    
    printf("Clubs sorted successfully.\n");
}
