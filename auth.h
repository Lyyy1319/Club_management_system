#ifndef AUTH_H
#define AUTH_H

#include "models.h"

// Hash a password using SHA-256
char* hash_password(const char* password);

// Register a new user
bool register_user(const char* username, const char* password, Role role);

// Login a user
User* login_user(const char* username, const char* password);

// Find user by username
User* find_user_by_username(const char* username);

#endif