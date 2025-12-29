#ifndef UTILS_H
#define UTILS_H

#include "models.h"

// Persistence functions
void save_data();
void load_data();

// Helper function to find club by id
Club* find_club(int club_id);

#endif