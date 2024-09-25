#ifndef G_CLAIMS_H
#define G_CLAIMS_H
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "G_Users.h"


char *priority_to_str(ClaimPriority priority);
char *cat_to_str(Category cat);
char *status_to_str(ClaimStatus status);
char *format_time(time_t time);

void displayAllClaims();

int generateClaimID();
void clientSubmitClaim(const User* uzar,  char* reason,  char* description,  Category category);
void ClientDisplayClaims(User user );
//--- Commun Functions ---
void modifyClaim(User user);
void deleteClaim(User user);
// --- Admin and Agent functions ---
void displayAllClaims();
void searchClaimsByCat_Stat(UserRole role);
int isClaimOwner(const char* username, int claimID);
int isWithinTimeLimit(time_t submissionDate);
void displayPendingClaims();
void printClaimDetails(Claim* claim);
void processPendingClaim(UserRole userRole);
// --- Admin-exclusive functions ---
void viewComplaintStatistics(UserRole userRole);
// Priority-related functions
void calculateClaimPriority(Claim* claim, ScoredClaim* scoredClaim);
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
