#ifndef G_USERS_H
#define G_USERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>
#define LOCKOUT_DURATION 300 // 5 minutes in seconds
#define MAX_USERS 100
#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define USER_FILE "users.txt"
#define MAX_CUSTOMER_NAME_LENGTH 50
#define MAX_REASON_LENGTH 100
#define MAX_DESCRIPTION_LENGTH 200
#define MAX_CATEGORY_LENGTH 50
#define MAX_KEYWORDS 15




typedef enum {
    STATUS_PENDING,
    STATUS_IN_PROGRESS,
    STATUS_RESOLVED,
} ClaimStatus;

typedef enum {
    Cat_TECHNECAL,
    Cat_FINANCIAL,
    Cat_SERVICE,
} Category;

typedef enum
{
    ROLE_CLIENT,
    ROLE_AGENT,
    ROLE_ADMIN
} UserRole;

typedef enum {
    PRIORITY_LOW,
    PRIORITY_MEDIUM,
    PRIORITY_HIGH
} ClaimPriority;

typedef struct {
    int claimID;
    char customerName[MAX_CUSTOMER_NAME_LENGTH];
    char reason[MAX_REASON_LENGTH];
    char description[MAX_DESCRIPTION_LENGTH];
     Category category;
    ClaimStatus status;
    ClaimPriority priority;
    time_t submissionDate;
    time_t procssetionDate;
} Claim;

// New struct to store claim and its associated score
typedef struct {
    Claim claim;
    int score;  // Score based on keywords
} ScoredClaim;

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
void deleteUser(UserRole userRole);
void searchUser(UserRole userRole);
int isPasswordValid(const char* password, const char* username);
int signUp(const char* username, const char* password);
int signIn(const char* username, const char* password);
void changeUserRole(UserRole UserRole);
User* authenticateUser(void);

#endif // G_USERS_H
