#ifndef G_CLAIMS_H
#define G_CLAIMS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "G_Users.h"

#define MAX_CLAIMS 100
#define MAX_CUSTOMER_NAME_LENGTH 50
#define MAX_REASON_LENGTH 100
#define MAX_DESCRIPTION_LENGTH 500
#define MAX_CATEGORY_LENGTH 50
#define CLAIM_FILE "claims.txt"
#define MAX_KEYWORDS 10 // Number of keywords in the table





int generateClaimID();
void clientSubmitClaim(const char* customerName,  char* reason,  char* description,  char* category);
void ClientDisplayClaims(char username );

//--- Commun Functions ---
void modifyClaim(User user);
void deleteClaim(User user);

// --- Admin and Agent functions ---
void displayAllClaims();
void searchClaimsByCat_Stat(void);
int isClaimOwner(const char* username, int claimID);
int isWithinTimeLimit(time_t submissionDate);
void displayPendingClaims();
void printClaimDetails(Claim* claim);
void processPendingClaim(UserRole userRole);


// --- Admin-exclusive functions ---
void viewComplaintStatistics(UserRole userRole);
// Priority-related functions
void calculateClaimPriority(Claim* claim);
void assignPriority(Claim* claim, ScoredClaim* scoredClaim);
void displayClaim(Claim claim);
void displayByPriority(UserRole userRole);
void viewComplaintStatistics(UserRole userRole);
void searchByStatus(char* status);
void searchByCategory(char* category);
void searchClaims(UserRole userRole, char* criteria);
time_t parseDate(const char* dateStr);
int isNumeric(const char* str);
void performDelete(int claimID);

// Save and load functions
void saveClaims(void);
void loadClaims(void);

#endif // G_CLAIMS_H
