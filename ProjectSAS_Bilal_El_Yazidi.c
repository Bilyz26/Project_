#include <stdio.h>
#include "G_Claims.h"
#include "G_Users.h"





void AdminDisplayMenu(User uzar) {   
   
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
        changeUserRole(uzar.role);

            break;
        case 2:
        displayAllClaims(uzar.role);
            break;
        case 3:
        modifyClaim(uzar);
            break;
        case 4:
        deleteClaim(uzar);
            break;

        case 5:
        processPendingClaim(uzar.role);
            break;

        case 6:
            char criteria[50];
            printf("Enter search criteria: ");
            scanf("%s", criteria);
            searchClaims(uzar.role,criteria);
            break;
        
        case 7:
             searchClaimsByCat_Stat();
            break;

        case 8:
            displayByPriority(uzar.role);
            break;

        case 9:
            viewComplaintStatistics(uzar.role);
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

void ClientDisplayMenu(User uzar) {
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
clientSubmitClaim(uzar.role);
            break;
        case 2:
        ClientDisplayClaims(uzar.username);
            break;
        case 3:
        deleteClaim(uzar.role);
            break;
        case 4:
        modifyClaim(uzar.role);
            break;
        case 5:
            printf("Exiting...\n");
            break;
        default:
            printf("Invalid choice. Please try again.\n");
            break;
    }
}

void AgentDisplayMenu(User uzar) {
    // Display agent-specific options
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
        displayAllClaims(uzar.role);
            break;
        case 2:
        modifyClaim(uzar);
            break;
        case 3:
        deleteClaim(uzar);
            break;

        case 4:
        processPendingClaim(uzar.role);
            break;

        case 5:
            char criteria[50];
            printf("Enter search criteria: ");
            scanf("%s", criteria);
            searchClaims(uzar.role,criteria);
            break;
        
        case 6:
             searchClaimsByCat_Stat();
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
    uzar authenticateUser();
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