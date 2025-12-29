#include <stdio.h>
#include <stdlib.h>
#include "club.h"
#include "models.h"
#include "utils.h"

// Helper function to find member in club
Member* find_member(Club *club, int user_id) {
    Member *m = club->members;
    while (m && m->user_id != user_id) m = m->next;
    return m;
}

void add_member_to_club(int club_id, int user_id, Role role) {
    Club *club = find_club(club_id);
    if (!club) {
        printf("Club not found.\n");
        return;
    }
    if (find_member(club, user_id)) {
        printf("User is already a member of this club.\n");
        return;
    }
    Member *new_member = malloc(sizeof(Member));
    new_member->user_id = user_id;
    new_member->role_in_club = role;
    new_member->next = club->members;
    club->members = new_member;
    printf("Member added successfully.\n");
}

void remove_member_from_club(int club_id, int user_id) {
    Club *club = find_club(club_id);
    if (!club) {
        printf("Club not found.\n");
        return;
    }
    Member *prev = NULL;
    Member *curr = club->members;
    while (curr && curr->user_id != user_id) {
        prev = curr;
        curr = curr->next;
    }
    if (!curr) {
        printf("Member not found in this club.\n");
        return;
    }
    if (prev) {
        prev->next = curr->next;
    } else {
        club->members = curr->next;
    }
    free(curr);
    printf("Member removed successfully.\n");
}

void change_member_role(int club_id, int user_id, Role new_role) {
    Club *club = find_club(club_id);
    if (!club) {
        printf("Club not found.\n");
        return;
    }
    Member *member = find_member(club, user_id);
    if (!member) {
        printf("Member not found in this club.\n");
        return;
    }
    member->role_in_club = new_role;
    printf("Member role changed successfully.\n");
}