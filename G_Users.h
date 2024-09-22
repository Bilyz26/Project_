#ifndef G_USERS_H
#define G_USERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_USERS 100
#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define USER_FILE "users.dat"
#define LOCKOUT_DURATION 1800 // 5 minutes in seconds

typedef enum {
    ROLE_CLIENT,
    ROLE_AGENT,
    ROLE_ADMIN
} UserRole;

typedef struct {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    UserRole role;
    int loginAttempts;
    time_t lockoutTime;
} User;

// Function declarations
void saveUsers(void);
void loadUsers(void);
int isPasswordValid(const char* password, const char* username);
int signUp(const char* username, const char* password);
int signIn(const char* username, const char* password);
void changeUserRole(int adminIndex);
User* authenticateUser(void);

#endif // G_USERS_H