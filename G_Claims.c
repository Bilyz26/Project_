
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

#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 50

typedef enum
{
    ROLE_CLIENT,
    ROLE_AGENT,
    ROLE_ADMIN
} UserRole;

typedef struct
{
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    UserRole role;
    int loginAttempts;
    time_t lockoutTime;
} User;

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


// Global array to store claims and a count of current claims
Claim claims[MAX_CLAIMS];
int claimCount = 0;

void saveClaims()
{
    FILE *file = fopen("claims.dat", "wb");
    if (file == NULL)
    {
        fprintf(stderr, "Error: Unable to open file for writing claims.\n");
        return;
    }
    if (fwrite(&claimCount, sizeof(int), 1, file) != 1)
    {
        fprintf(stderr, "Error: Failed to write claim count.\n");
        fclose(file);
        return;
    }
    if (fwrite(claims, sizeof(Claim), claimCount, file) != claimCount)
    {
        fprintf(stderr, "Error: Failed to write claim data.\n");
    }
    fclose(file);
    printf("Claim data saved successfully.\n");
}

void loadClaims()
{
    FILE *file = fopen("claims.dat", "rb");
    if (file == NULL)
    {
        printf("No existing claim data found. Starting with an empty claim list.\n");
        return;
    }
    if (fread(&claimCount, sizeof(int), 1, file) != 1)
    {
        fprintf(stderr, "Error: Failed to read claim count.\n");
        fclose(file);
        return;
    }
    if (fread(claims, sizeof(Claim), claimCount, file) != claimCount)
    {
        fprintf(stderr, "Error: Failed to read claim data.\n");
    }
    fclose(file);
    printf("Claim data loaded successfully.\n");
}


// Function to auto-generate a unique claim ID
int generateClaimID() {
    if (claimCount == 0) {
        return 1;  // Start at 1 if no claims exist
    } else {
        return claims[claimCount - 1].claimID + 1;  // Increment from the last ID
    }
}

// Client-specific function to submit a claim
void clientSubmitClaim(const char* customerName, char* reason, char* description, char* category) {
    if (claimCount >= MAX_CLAIMS) {
        printf("Cannot submit claim. Maximum number of claims reached.\n");
        return;
    }

    Claim newClaim;
    newClaim.claimID = generateClaimID();  // Auto-incremented ID
    strncpy(newClaim.customerName, customerName, MAX_CUSTOMER_NAME_LENGTH);
    strncpy(newClaim.reason, reason, MAX_REASON_LENGTH);
    strncpy(newClaim.description, description, MAX_DESCRIPTION_LENGTH);
    strncpy(newClaim.category, category, MAX_CATEGORY_LENGTH);
    newClaim.status = STATUS_PENDING;  // Initially, the status is set to pending
    newClaim.priority = PRIORITY_LOW;  // Default priority (client cannot manually set this)
    newClaim.submissionDate = time(NULL);  // Set the submission date to current time
    newClaim.procssetionDate = 0;  // Processing date left for admins or agents

    // Calculate and assign priority based on the description
    ScoredClaim scoredClaim;
    calculateClaimPriority(&newClaim, &scoredClaim);
    assignPriority(&newClaim, &scoredClaim);

    // Save the new claim to the claims array
    claims[claimCount] = newClaim;
    claimCount++;

    printf("Claim submitted successfully with ID: %d and Priority: %d\n", newClaim.claimID, newClaim.priority);
    saveClaims();  // Save the claim to the file
}



void ClientDisplayClaims(char* username) {

    // Load claims data before displaying
    loadClaims(); // Ensure you have a function to load claims data

    printf("Claims for user: %s\n", username);
    int found = 0;

    for (int i = 0; i < claimCount; i++) {
        if (claims[i].claimID != 0 && strcmp(claims[i].customerName, username) == 0) {
            printf("Claim ID: %d\n", claims[i].claimID);
            printf("Reason: %s\n", claims[i].reason);
            printf("Description: %s\n", claims[i].description);
            printf("Category: %s\n", claims[i].category);
            printf("Status: %d\n", claims[i].status); // Consider using an enum to display readable status
            printf("Submission Date: %s\n", ctime(&claims[i].submissionDate));
            found = 1;
        }
    }

    if (!found) {
        printf("No claims found for user %s.\n", username);
    }
}

// Function to display all claims
void displayAllClaims() {
    printf("All Claims:\n");
    for (int i = 0; i < claimCount; i++) {
        printf("ID: %d, Customer Name: %s, Reason: %s, Status: %d\n",
               claims[i].claimID, claims[i].customerName, claims[i].reason, claims[i].status);
    }
}

//--- Commun Functions ---
// Helper function to check if the user is the owner of the claim
int isClaimOwner(const char* username, int claimID) {
    for (int i = 0; i < claimCount; i++) {
        if (claims[i].claimID == claimID && strcmp(claims[i].customerName, username) == 0) {
            return 1; // User is the owner
        }
    }
    return 0; // User is not the owner
}

// Helper function to check if the claim is within the 24-hour limit
int isWithinTimeLimit(time_t submissionDate) {

    time_t currentTime = time(NULL);
    double secondsSinceSubmission = difftime(currentTime, submissionDate);
    return secondsSinceSubmission <= 86400; // 86400 seconds in 24 hours
}

void modifyClaim(int claimID, UserRole userRole, const char* username) {
    // Display user's claims before modification
    if (userRole == ROLE_CLIENT) {
        ClientDisplayClaims((char*)username); // Cast to char* for compatibility
    } else {
        displayAllClaims(); // Assuming this function displays all claims for admins/agents
    }

    // Find the claim by claimID
    Claim* claimToModify = NULL;
    for (int i = 0; i < claimCount; i++) {
        if (claims[i].claimID == claimID) {
            claimToModify = &claims[i];
            break;
        }
    }

    // If claim is not found
    if (!claimToModify) {
        printf("Claim with ID %d not found.\n", claimID);
        return;
    }

    // Check user role for modifications
    if (userRole == ROLE_CLIENT) {
        // Ensure the client can only modify their own claims
        if (!isClaimOwner(username, claimID)) {
            printf("You can only modify your own claims.\n");
            return;
        }

        // Check if it's within 24 hours of submission
        if (!isWithinTimeLimit(claimToModify->submissionDate)) {
            printf("You can only modify your claim within 24 hours of submission.\n");
            return;
        }
    }

    // Display the current claim details
    printf("Current Claim Details:\n");
    printf("ID: %d\n", claimToModify->claimID);
    printf("Customer Name: %s\n", claimToModify->customerName);
    printf("Reason: %s\n", claimToModify->reason);
    printf("Description: %s\n", claimToModify->description);
    printf("Category: %s\n", claimToModify->category);
    printf("Status: %d\n", claimToModify->status);
    
    // Get new details from the user
    char newReason[MAX_REASON_LENGTH];
    char newDescription[MAX_DESCRIPTION_LENGTH];
    char newCategory[MAX_CATEGORY_LENGTH];

    printf("Enter new reason (or press Enter to keep current): ");
    fgets(newReason, sizeof(newReason), stdin);
    newReason[strcspn(newReason, "\n")] = 0;  // Remove trailing newline

    printf("Enter new description (or press Enter to keep current): ");
    fgets(newDescription, sizeof(newDescription), stdin);
    newDescription[strcspn(newDescription, "\n")] = 0;

    printf("Enter new category (or press Enter to keep current): ");
    fgets(newCategory, sizeof(newCategory), stdin);
    newCategory[strcspn(newCategory, "\n")] = 0;

    // Update only if new input is provided
    if (strlen(newReason) > 0) {
        strncpy(claimToModify->reason, newReason, MAX_REASON_LENGTH);
    }
    if (strlen(newDescription) > 0) {
        strncpy(claimToModify->description, newDescription, MAX_DESCRIPTION_LENGTH);
    }
    if (strlen(newCategory) > 0) {
        strncpy(claimToModify->category, newCategory, MAX_CATEGORY_LENGTH);
    }

    printf("Claim modified successfully.\n");
    saveClaims();  // Save the updated claims
}


// Helper function to perform the delete operation
void performDelete(int claimID) {
    for (int i = 0; i < claimCount; i++) {
        if (claims[i].claimID == claimID) {
            // Shift claims to remove the deleted claim
            for (int j = i; j < claimCount - 1; j++) {
                claims[j] = claims[j + 1];
            }
            claimCount--; // Reduce the count of claims
            printf("Claim ID %d deleted successfully.\n", claimID);
            return;
        }
    }
    printf("Claim ID %d not found.\n", claimID);
}

// Main delete function
void deleteClaim(int claimID, const char* username, UserRole role) {
    if (role == ROLE_CLIENT) {
        if (!isClaimOwner(username, claimID)) {
            printf("You can only delete your own claims.\n");
            return;
        }

        // Check if within 24 hours
        for (int i = 0; i < claimCount; i++) {
            if (claims[i].claimID == claimID) {
                if (!isWithinTimeLimit(claims[i].submissionDate)) {
                    printf("You can only delete claims within 24 hours of submission.\n");
                    return;
                }
                break; // Found the claim and checked the time limit
            }
        }
    }

    // If role is admin or agent, they can delete any claim
    performDelete(claimID);
}

// Keywords table for scanning the claim description
const char* keywordTable[MAX_KEYWORDS] = {
    "urgent", "critical", "immediate", "important", 
    "failure", "broken", "damaged", "error", 
    "issue", "problem"
};

// Function to calculate the score of a claim based on keywords---------------------------------------------------------------****////
void calculateClaimPriority(Claim* claim, ScoredClaim* scoredClaim) {
    // List of keywords to search for in the claim's description
    const char* keywords[MAX_KEYWORDS] = { "urgent", "critical", "immediate", "important", "high-priority", "emergency", "issue", "problem", "failure", "escalation" };
    
    // Initialize score to 0
    scoredClaim->score = 0;
    scoredClaim->claim = *claim;  // Assign the claim to the scored claim struct

    // Check how many keywords are present in the claim's description
    for (int i = 0; i < MAX_KEYWORDS; i++) {
        if (strstr(claim->description, keywords[i]) != NULL) {
            scoredClaim->score++;  // Increment score for each keyword found
        }
    }
}

void assignPriority(Claim* claim, ScoredClaim* scoredClaim) {
    // Example scoring thresholds for assigning priorities
    int score = scoredClaim->score;

    if (score <= 2) {
        claim->priority = PRIORITY_LOW;  // Low priority if 2 or fewer keywords found
    } else if (score <= 6) {
        claim->priority = PRIORITY_MEDIUM;  // Medium priority if 3-6 keywords found
    } else {
        claim->priority = PRIORITY_HIGH;  // High priority if more than 6 keywords found
    }
}


