#ifndef G_CLAIMS_H
#define G_CLAIMS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CLAIMS 100
#define MAX_CUSTOMER_NAME_LENGTH 50
#define MAX_REASON_LENGTH 100
#define MAX_DESCRIPTION_LENGTH 500
#define MAX_CATEGORY_LENGTH 50
#define CLAIM_FILE "claims.dat"
#define MAX_KEYWORDS 10 // Number of keywords in the table


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
    char category[MAX_CATEGORY_LENGTH];
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

// Function declarations

// --- Client-specific functions ---
int generateClaimID();
void clientSubmitClaim(const char* customerName,  char* reason,  char* description,  char* category);
void ClientDisplayClaims(char username );

//--- Commun Functions ---
void modifyClaim(int claimID);
void deleteClaim(int claimID);

// --- Admin and Agent functions ---
void displayAllClaims();
void processClaim(int claimID, ClaimStatus newStatus);
void searchClaims(const char* criteria);
void searchClaimsByCat_Stat(void);



// --- Admin-exclusive functions ---

// Priority-related functions
void calculateClaimPriority(Claim* claim);
void assignPriority(Claim* claim, ScoredClaim* scoredClaim);


// Save and load functions
void saveClaims(void);
void loadClaims(void);

#endif // G_CLAIMS_H
