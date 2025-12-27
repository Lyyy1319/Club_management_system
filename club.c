#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "club.h"
#include "models.h"

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
}

// Remove member from club
void remove_member_from_club(int club_id, int user_id) {
    Club* c = find_club_by_id(club_id);
    if (!c) return;

    remove_member_from_club_list(c, user_id);
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