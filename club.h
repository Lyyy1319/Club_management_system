#ifndef CLUB_H
#define CLUB_H

#include "models.h"

// Member management functions
void add_member_to_club(int club_id, int user_id, Role role);
void remove_member_from_club(int club_id, int user_id);
void change_member_role(int club_id, int user_id, Role new_role);

#endif