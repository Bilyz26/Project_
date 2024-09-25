#include <stdio.h>
#include "G_Claims.h"
#include "G_Users.h"
#include <stdio.h>
#include <string.h>
#include "G_Users.h"
#include <stdbool.h>



void AdminDisplayMenu(User* uzar) {
   char criteria[50];
  printf("Admin Menu:\n");
    printf("1. Change User Role\n");
    printf("2. display All Claims\n");
    printf("3. Modify Claim\n");
    printf("4. Delete Claim\n");
    printf("5. Process Pending Claims\n");
    printf("6. Search Claim by (ID/Username/Submition date)\n");
    printf("7. Search Claim by (Status/Category)\n");
    printf("8. Display Claims Ordered By Priority\n");
    printf("9. Statistics\n");
    printf("10. Daily Report\n");
    printf("11. Exit\n");

    int choice;
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
        changeUserRole(uzar->role);

            break;
        case 2:
        displayAllClaims();
            break;
        case 3:
        modifyClaim(*uzar);
            break;
        case 4:
        deleteClaim(*uzar);
            break;

        case 5:
        processPendingClaim(uzar->role);
            break;

        case 6:

            printf("Enter search criteria: ");
            scanf("%s", criteria);
            searchClaims(uzar->role,criteria);
            break;

        case 7:
             searchClaimsByCat_Stat(uzar->role);
            break;

        case 8:
            displayByPriority(uzar->role);
            break;

        case 9:
            viewComplaintStatistics(uzar->role);
            break;

        case 10:

            break;

        case 11:
            printf("Exiting...\n");
            break;
        default:
            printf("Invalid choice. Please try again.\n");
            break;
    }
}

void ClientDisplayMenu(User* uzar) {
    char customerName[MAX_USERNAME_LENGTH];
    char reason[MAX_REASON_LENGTH];
    char description[MAX_DESCRIPTION_LENGTH];
    int categoryChoice;
    Category category;
    // Display client-specific options
    printf("Client Menu:\n");
    printf("1. Submit a Claim\n");
    printf("2. View My Claims\n");
    printf("3. Delete A Claim\n");
    printf("4. Modify A Claim\n");
    printf("5. Exit\n");

    int choice;
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:

            printf("Enter your name: ");
    fgets(customerName, MAX_USERNAME_LENGTH, stdin);
    customerName[strcspn(customerName, "\n")] = 0;  // Remove newline character

    printf("Enter reason: ");
    fgets(reason, MAX_REASON_LENGTH, stdin);
    reason[strcspn(reason, "\n")] = 0;  // Remove newline character

    printf("Enter description: ");
    fgets(description, MAX_DESCRIPTION_LENGTH, stdin);
    description[strcspn(description, "\n")] = 0;  // Remove newline character

    printf("Select category (1 for Technical, 2 for Financial, 3 for Service): ");
    scanf("%d", &categoryChoice);

    switch (categoryChoice) {
        case 1:
            category = Cat_TECHNECAL;
            break;
        case 2:
            category = Cat_FINANCIAL;
            break;
        case 3:
            category = Cat_SERVICE;
            break;
        default:
            printf("Invalid category choice. Defaulting to Technical.\n");
            category = Cat_TECHNECAL;
    }

clientSubmitClaim(uzar,reason, description, category);
            break;
        case 2:
        ClientDisplayClaims(*uzar);
            break;
        case 3:
        deleteClaim(*uzar);
            break;
        case 4:
        modifyClaim(*uzar);
            break;
        case 5:
            printf("Exiting...\n");
            break;
        default:
            printf("Invalid choice. Please try again.\n");
            break;
    }
}

void AgentDisplayMenu(User* uzar) {
    // Display agent-specific options
    char criteria[50];
    printf("Agent Menu:\n");
    printf("1. display All Claims\n");
    printf("2. Modify Claim\n");
    printf("3. Delete Claim\n");
    printf("4. Process Pending Claims\n");
    printf("5. Search Claim by (ID/Username/Submition date)\n");
    printf("6. Search Claim by (Status/Category)\n");
    printf("7. Exit\n");

    int choice;
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
case 1:
        displayAllClaims();
            break;
        case 2:
        modifyClaim(*uzar);
            break;
        case 3:
        deleteClaim(*uzar);
            break;

        case 4:
        processPendingClaim(uzar->role);
            break;

        case 5:

            printf("Enter search criteria: ");
            scanf("%s", criteria);
            searchClaims(uzar->role,criteria);
            break;

        case 6:
             searchClaimsByCat_Stat(uzar->role);
            break;

        case 7:
            printf("Exiting...\n");
            break;
        default:
            printf("Invalid choice. Please try again.\n");
            break;
    }
}

User* uzar;
int main() {

    uzar = authenticateUser();
    if(uzar->role==ROLE_ADMIN)
    {
        AdminDisplayMenu(uzar);
    }
    else if(uzar->role==ROLE_AGENT)
    {
        AgentDisplayMenu(uzar);

    }else
    {
        ClientDisplayMenu(uzar);

    }
    return 0;
}
