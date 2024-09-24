
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

void displayPendingClaims()
{
    int found = 0;
    printf("Pending Claims:\n");
    printf("ID\tCustomer Name\tReason\t\tDescription\t\tCategory\t\tStatus\n");
    printf("------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < claimCount; i++) {
        if (claims[i].status == STATUS_PENDING) {
            found = 1;
            printf("%d\t%s\t%s\t%s\t%s\t%d\n", claims[i].claimID, claims[i].customerName, 
                   claims[i].reason, claims[i].description, claims[i].category, claims[i].status);
        }
    }

    if (!found) {
        printf("No pending claims found.\n");
    }
}

void processPendingClaim(UserRole userRole)
{
    // Ensure only admins or agents can process claims
    if (userRole != ROLE_ADMIN && userRole != ROLE_AGENT) {
        printf("You do not have permission to process claims.\n");
        return;
    }

    // Display all pending claims first
    displayPendingClaims();

    // Ask the user to choose a claim ID to process
    int claimID;
    printf("Enter the claim ID of the pending claim to process: ");
    scanf("%d", &claimID);
    getchar();  // Clear the newline character from the buffer

    // Find the claim by its ID
    Claim* claimToProcess = NULL;
    for (int i = 0; i < claimCount; i++) {
        if (claims[i].claimID == claimID) {
            claimToProcess = &claims[i];
            break;
        }
    }

    // If claim is not found
    if (!claimToProcess) {
        printf("Claim with ID %d not found.\n", claimID);
        return;
    }

    // Ensure the claim is in a pending state
    if (claimToProcess->status != STATUS_PENDING) {
        printf("Claim with ID %d is not pending and cannot be processed.\n", claimID);
        return;
    }

    // Allow the user to select the new status
    int statusChoice;
    printf("Choose the new status for the claim:\n");
    printf("1. In Progress\n");
    printf("2. Resolved\n");
    printf("3. Rejected\n");
    printf("Enter your choice (1-3): ");
    scanf("%d", &statusChoice);
    getchar();  // Clear the newline character from the buffer

    // Set the new status based on the user's choice
    ClaimStatus newStatus;
    switch (statusChoice) {
        case 1:
            newStatus = STATUS_IN_PROGRESS;
            break;
        case 2:
            newStatus = STATUS_RESOLVED;
            break;
        case 3:
            // Assuming we want to treat rejected claims similarly to resolved ones
            newStatus = STATUS_RESOLVED;
            break;
        default:
            printf("Invalid status choice. Operation cancelled.\n");
            return;
    }

    // Update the claim's status
    claimToProcess->status = newStatus;
    claimToProcess->procssetionDate=time(NULL);

    // Ask for internal notes about the processing steps
    char internalNotes[200];
    printf("Enter internal notes about the processing steps (max 200 characters): ");
    fgets(internalNotes, sizeof(internalNotes), stdin);
    internalNotes[strcspn(internalNotes, "\n")] = 0;  // Remove trailing newline

    // You can store or display these notes if necessary.

    // Update the processing date
    claimToProcess->procssetionDate = time(NULL);

    // Display success message
    printf("Claim ID %d processed. New Status: %d\n", claimID, newStatus);
    printf("Internal Notes: %s\n", internalNotes);

    // Save the updated claims to file
    saveClaims();
}

void printClaimDetails(Claim* claim) {
    char submissionDateStr[100];
    strftime(submissionDateStr, sizeof(submissionDateStr), "%Y-%m-%d", localtime(&claim->submissionDate));

    printf("Claim ID: %d\n", claim->claimID);
    printf("Customer Name: %s\n", claim->customerName);
    printf("Reason: %s\n", claim->reason);
    printf("Description: %s\n", claim->description);
    printf("Category: %s\n", claim->category);
    printf("Submission Date: %s\n", submissionDateStr);
    printf("-----------------------------\n");
}

// Helper function to check if a string is a numeric value (for Claim ID search)
int isNumeric(const char* str) {
    while (*str) {
        if (!isdigit(*str)) return 0;
        str++;
    }
    return 1;
}

// Helper function to parse a date in "YYYY-MM-DD" format
time_t parseDate(const char* dateStr) {
    struct tm tm = {0};
    if (strptime(dateStr, "%Y-%m-%d", &tm) != NULL) {
        return mktime(&tm);
    }
    return (time_t)-1; // Return invalid time if parsing fails
}

void searchClaims(UserRole userRole, char* criteria) {
    if (userRole != ROLE_ADMIN && userRole != ROLE_AGENT) {
        printf("You do not have permission to search claims.\n");
        return;
    }

    int found = 0;

    // Check if the search criteria is numeric (possibly claim ID)
    if (isNumeric(criteria)) {
        int searchID = atoi(criteria);
        for (int i = 0; i < claimCount; i++) {
            if (claims[i].claimID == searchID) {
                printClaimDetails(&claims[i]);
                found = 1;
            }
        }
    }
    // Check if the search criteria is a valid date
    else if (parseDate(criteria) != (time_t)-1) {
        time_t searchDate = parseDate(criteria);
        for (int i = 0; i < claimCount; i++) {
            if (difftime(claims[i].submissionDate, searchDate) == 0) {
                printClaimDetails(&claims[i]);
                found = 1;
            }
        }
    }
    // Otherwise, treat the search criteria as a customer name
    else {
        for (int i = 0; i < claimCount; i++) {
            if (strstr(claims[i].customerName, criteria) != NULL) {
                printClaimDetails(&claims[i]);
                found = 1;
            }
        }
    }

    if (!found) {
        printf("No claims found matching the criteria: %s\n", criteria);
    }
}


// Helper function to search claims by category
void searchByCategory(char* category) {
    int found = 0;
    printf("Searching for claims in the category: %s\n", category);
    for (int i = 0; i < claimCount; i++) {
        if (strcmp(claims[i].category, category) == 0) {
            // Display claim if category matches
            displayClaim(claims[i]);
            found++;
        }
    }
    if (found == 0) {
        printf("No claims found in the category: %s\n", category);
    }
}

// Helper function to search claims by status
void searchByStatus(char* status) {
    int found = 0;
    printf("Searching for claims with status: %s\n", status);
    for (int i = 0; i < claimCount; i++) {
        if (strcmp(claims[i].status, status) == 0) {
            // Display claim if status matches
            displayClaim(claims[i]);
            found++;
        }
    }
    if (found == 0) {
        printf("No claims found with status: %s\n", status);
    }
}

// Function to display claim details (to be implemented based on your structure)
void displayClaim(Claim claim) {

    printf("Claim ID: %d\n", claim.claimID);
    printf("Category: %s\n", claim.category);
    printf("Status: %s\n", claim.status);
    printf("Description: %s\n", claim.description);
    printf("Date Submitted: %s\n", claim.submissionDate);
    // Add more fields if necessary
    printf("-----------------------------------\n");
}


void searchClaimsByCat_Stat(UserRole userRole) {
    // Ensure only Admins and Agents can access this function
    if (userRole == ROLE_ADMIN || userRole == ROLE_AGENT) {
        int searchChoice;
        printf("Choose your search option:\n");
        printf("1. Search by Category\n");
        printf("2. Search by Status\n");
        printf("Enter your choice (1 or 2): ");
        scanf("%d", &searchChoice);

        if (searchChoice == 1) {
            // Search by Category
            char category[50];
            printf("Enter the claim category to search: ");
            scanf("%s", category);
            searchByCategory(category);  // Function to search claims by category
        } else if (searchChoice == 2) {
            // Search by Status
            int statusChoice;
            printf("Choose the status to search by:\n");
            printf("1. In Progress\n");
            printf("2. Rejected\n");
            printf("3. Resolved\n");
            printf("Enter your choice (1, 2, or 3): ");
            scanf("%d", &statusChoice);
            
            switch (statusChoice) {
                case 1:
                    searchByStatus("In Progress");  // Function to search claims with "In Progress" status
                    break;
                case 2:
                    searchByStatus("Rejected");  // Function to search claims with "Rejected" status
                    break;
                case 3:
                    searchByStatus("Resolved");  // Function to search claims with "Resolved" status
                    break;
                default:
                    printf("Invalid choice.\n");
                    break;
            }
        } else {
            printf("Invalid choice. Please select either 1 or 2.\n");
        }
    } else {
        printf("You do not have permission to access this feature.\n");
    }
}