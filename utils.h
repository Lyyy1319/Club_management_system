#ifndef UTILS_H
#define UTILS_H

#include <time.h>
#include <stddef.h>
#include "models.h"

// Persistence functions
void save_data();
void load_data();

// Operation log
void log_action(const char *fmt, ...);

// Backup & restore
// Creates a timestamped backup directory under `backups/` and copies data files there.
// Returns 0 on success, non-zero on error.
int create_backup();
// Restores data files from the specified backup directory (e.g. backups/backup_20251230_123456)
// Returns 0 on success, non-zero on error.
int restore_backup(const char *backup_dir);

// Helper function to find club by id
Club* find_club(int club_id);

// Fuzzy search / similarity utilities
int levenshtein_distance(const char *s1, const char *s2);
int fuzzy_match(const char *s, const char *pattern, int max_distance);

// Time parsing helper for activities
time_t parse_time_str(const char *time_str);

#endif