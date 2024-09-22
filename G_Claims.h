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
    STATUS_REJECTED
} ClaimStatus;

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
} Claim;

// New struct to store claim and its associated score
typedef struct {
    Claim claim;
    int score;  // Score based on keywords
} ScoredClaim;

// Function declarations

// --- Client-specific functions ---
void clientSubmitClaim(const char* customerName, const char* reason, const char* description, const char* category);
void displayClaims(char username );

//--- Commun Functions ---
void modifyClaim(int claimID);
void deleteClaim(int claimID);

// --- Admin and Agent functions ---
void displayClaims(void);
void processClaim(int claimID, ClaimStatus newStatus);
void searchClaims(const char* criteria);

// --- Admin-exclusive functions ---
void listClaimsByPriority(void);
void restrictToAdmin(const char* agentName);

// Priority-related functions
void calculateClaimPriority(Claim* claim, ScoredClaim* scoredClaim);
void assignPriority(ScoredClaim* scoredClaim);

// Save and load functions
void saveClaims(void);
void loadClaims(void);

#endif // G_CLAIMS_H
