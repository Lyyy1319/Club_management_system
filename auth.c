#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "models.h"
#include "auth.h"

// Global head for users (defined in models.h)
extern User* users_head;

static int next_user_id = 1;

// Simple hash function (djb2)
unsigned long hash_password_simple(const char* password) {
    unsigned long hash = 5381;
    int c;
    while ((c = *password++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

char* hash_password(const char* password) {
    unsigned long hash = hash_password_simple(password);
    char* str = (char*)malloc(21); // enough for unsigned long
    if (!str) return NULL;
    sprintf(str, "%lu", hash);
    return str;
}

User* find_user_by_username(const char* username) {
    User* current = users_head;
    while (current) {
        if (strcmp(current->username, username) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

bool register_user(const char* username, const char* password, Role role) {
    if (find_user_by_username(username)) {
        return false; // Username already exists
    }

    char* hashed = hash_password(password);
    if (!hashed) return false;

    User* new_user = (User*)malloc(sizeof(User));
    if (!new_user) {
        free(hashed);
        return false;
    }

    new_user->id = next_user_id++;
    new_user->username = (char*)malloc(strlen(username) + 1);
    if (!new_user->username) {
        free(hashed);
        free(new_user);
        return false;
    }
    strcpy(new_user->username, username);
    new_user->password_enc = hashed;
    new_user->role = role;
    new_user->next = users_head;
    users_head = new_user;

    return true;
}

User* login_user(const char* username, const char* password) {
    User* user = find_user_by_username(username);
    if (!user) return NULL;

    char* hashed_input = hash_password(password);
    if (!hashed_input) return NULL;

    bool match = strcmp(user->password_enc, hashed_input) == 0;
    free(hashed_input);

    return match ? user : NULL;
}