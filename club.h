#ifndef CLUB_H
#define CLUB_H

#include "models.h"

// CRUD operations for clubs
Club* create_club(char* name, char* found_date, int founder_id);
Club* find_club_by_id(int id);
void update_club(int id, char* name, char* found_date, int founder_id, double balance, int approved);
void delete_club(int id);

// Member management
void add_member_to_club(int club_id, int user_id, Role role);
void remove_member_from_club(int club_id, int user_id);
void change_member_role(int club_id, int user_id, Role new_role);

// Utility functions for linked list management
void insert_club_at_head(Club* new_club);
void remove_club_from_list(int id);
Member* find_member_in_club(Club* club, int user_id);
void insert_member_at_head(Club* club, Member* new_member);
void remove_member_from_club_list(Club* club, int user_id);

// Search and Sort
void search_clubs_fuzzy(const char* keyword);
void sort_clubs(int criteria); // 1: ID, 2: Name, 3: Balance

#endif