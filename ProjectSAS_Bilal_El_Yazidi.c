#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_USERS 100
#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define USER_FILE "users.dat"
#define MAX_CLAIMS 100
#define MAX_CLAIM_DESCRIPTION 256

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
} User;

typedef enum
{
    STATUS_PENDING,
    STATUS_IN_PROGRESS,
    STATUS_RESOLVED,
    STATUS_CLOSED
} ClaimStatus;

typedef struct
{
    int id;
    char description[MAX_CLAIM_DESCRIPTION];
    ClaimStatus status;
    char username[MAX_USERNAME_LENGTH]; // User who created the claim
} Claim;

User users[MAX_USERS];
Claim claims[MAX_CLAIMS];
int userCount = 0;
int claimCount = 0;

void saveUsers()
{
    FILE *file = fopen(USER_FILE, "wb");
    if (file == NULL)
    {
        fprintf(stderr, "Error: Unable to open file for writing.\n");
        return;
    }
    if (fwrite(&userCount, sizeof(int), 1, file) != 1)
    {
        fprintf(stderr, "Error: Failed to write user count.\n");
        fclose(file);
        return;
    }
    if (fwrite(users, sizeof(User), userCount, file) != userCount)
    {
        fprintf(stderr, "Error: Failed to write user data.\n");
    }
    fclose(file);
    printf("User data saved successfully.\n");
}

void loadUsers()
{
    FILE *file = fopen(USER_FILE, "rb");
    if (file == NULL)
    {
        printf("No existing user data found. Starting with an empty user list.\n");
        return;
    }
    if (fread(&userCount, sizeof(int), 1, file) != 1)
    {
        fprintf(stderr, "Error: Failed to read user count.\n");
        fclose(file);
        return;
    }
    if (fread(users, sizeof(User), userCount, file) != userCount)
    {
        fprintf(stderr, "Error: Failed to read user data.\n");
    }
    fclose(file);
    printf("User data loaded successfully.\n");
}


int isPasswordValid(const char* password, const char* username)
{
    int hasUpper = 0, hasLower = 0, hasDigit = 0, hasSpecial = 0;
    size_t len = strlen(password);

    // V�rification de la longueur minimale
    if (len < 8 || len >= MAX_PASSWORD_LENGTH)
    {
        printf("Le mot de passe doit contenir au moins 8 caract�res et ne pas d�passer %d caract�res.\n", MAX_PASSWORD_LENGTH - 1);
        return 0;
    }

    // V�rification des exigences de caract�res
    for (size_t i = 0; i < len; i++)
    {
        if (isupper(password[i])) hasUpper = 1;
        else if (islower(password[i])) hasLower = 1;
        else if (isdigit(password[i])) hasDigit = 1;
        else if (strchr("!@#$%^&*", password[i])) hasSpecial = 1;
    }

    // V�rification que le mot de passe ne contient pas le nom d'utilisateur
    if (strstr(password, username) != NULL)
    {
        printf("Le mot de passe ne doit pas contenir le nom d'utilisateur.\n");
        return 0;
    }

    // V�rification de toutes les contraintes
    if (!hasUpper)
        printf("Le mot de passe doit contenir au moins une lettre majuscule.\n");
    if (!hasLower)
        printf("Le mot de passe doit contenir au moins une lettre minuscule.\n");
    if (!hasDigit)
        printf("Le mot de passe doit contenir au moins un chiffre.\n");
    if (!hasSpecial)
        printf("Le mot de passe doit contenir au moins un caract�re sp�cial (!@#$%^&*).\n");

    // Retourne 1 si toutes les conditions sont remplies
    return (hasUpper && hasLower && hasDigit && hasSpecial);
}






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

int signUp(const char* username, const char* password)
{
    if (userCount >= MAX_USERS)
    {
        printf("Maximum user limit reached.\n");
        return 0;
    }

    for (int i = 0; i < userCount; i++)
    {
        if (strcmp(users[i].username, username) == 0)
        {
            printf("Username already exists.\n");
            return 0;
        }
    }

    // Validate the password before saving
    if (!isPasswordValid(password, username))
    {
        printf("Password validation failed. User not registered.\n");
        return 0;
    }

    // Save the username
    strncpy(users[userCount].username, username, MAX_USERNAME_LENGTH - 1);
    users[userCount].username[MAX_USERNAME_LENGTH - 1] = '\0';

    // Save the password
    strncpy(users[userCount].password, password, MAX_PASSWORD_LENGTH - 1);
    users[userCount].password[MAX_PASSWORD_LENGTH - 1] = '\0';

    // Assign role
    users[userCount].role = (userCount == 0) ? ROLE_ADMIN : ROLE_CLIENT;

    printf("User registered successfully as %s.\n",
           (userCount == 0) ? "Admin" : "Client");
    userCount++;
    return 1;
}

int signIn(const char* username, const char* password)
{
    for (int i = 0; i < userCount; i++)
    {
        if (strcmp(users[i].username, username) == 0)
        {
            if (strcmp(users[i].password, password) == 0)
            {
                printf("Login successful. Welcome, %s!\n", username);
                return i; // Return index of the user
            }
            else
            {
                printf("Incorrect password.\n");
                return -1;
            }
        }
    }
    printf("User not found.\n");
    return -1;
}

void changeUserRole(int adminIndex)
{
    if (users[adminIndex].role != ROLE_ADMIN)
    {
        printf("Only Administrators can change roles.\n");
        return;
    }

    char username[MAX_USERNAME_LENGTH];
    int newRole;

    printf("Enter username to change role: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0; // Remove newline

    for (int i = 0; i < userCount; i++)
    {
        if (strcmp(users[i].username, username) == 0)
        {
            while (1)
            {
                printf("Select new role (1: Client, 2: Agent, 3: Admin): ");
                scanf("%d", &newRole);
                getchar(); // Consume newline

                if (newRole < 1 || newRole > 3)
                {
                    printf("Invalid role choice. Please try again.\n");
                }
                else
                {
                    users[i].role = (UserRole)(newRole - 1);
                    printf("Role updated for user %s.\n", username);
                    return;
                }
            }
        }
    }
    printf("User not found.\n");
}

void submitClaim(const char* username, const char* description)
{
    if (claimCount >= MAX_CLAIMS)
    {
        printf("Maximum claim limit reached.\n");
        return;
    }

    claims[claimCount].id = claimCount + 1; // Simple incremental ID
    strncpy(claims[claimCount].description, description, MAX_CLAIM_DESCRIPTION - 1);
    claims[claimCount].description[MAX_CLAIM_DESCRIPTION - 1] = '\0';
    claims[claimCount].status = STATUS_PENDING;
    strncpy(claims[claimCount].username, username, MAX_USERNAME_LENGTH - 1);
    claims[claimCount].username[MAX_USERNAME_LENGTH - 1] = '\0';

    printf("Claim submitted successfully.\n");
    claimCount++;
}

void viewClaims()
{
    ///////////////////////
}

void changeClaimStatus(int claimId, ClaimStatus newStatus)
{
    for (int i = 0; i < claimCount; i++)
    {
        if (claims[i].id == claimId)
        {
            claims[i].status = newStatus;
            printf("Claim status updated to %d.\n", newStatus);
            return;
        }
    }
    printf("Claim not found.\n");
}

void modifyClaim(int claimId, const char* newDescription)
{
    for (int i = 0; i < claimCount; i++)
    {
        if (claims[i].id == claimId)
        {
            strncpy(claims[i].description, newDescription, MAX_CLAIM_DESCRIPTION - 1);
            claims[i].description[MAX_CLAIM_DESCRIPTION - 1] = '\0';
            printf("Claim modified successfully.\n");
            return;
        }
    }
    printf("Claim not found.\n");
}

void deleteClaim(int claimId)
{
    for (int i = 0; i < claimCount; i++)
    {
        if (claims[i].id == claimId)
        {
            // Shift claims to fill the gap
            for (int j = i; j < claimCount - 1; j++)
            {
                claims[j] = claims[j + 1];
            }
            claimCount--;
            printf("Claim deleted successfully.\n");
            return;
        }
    }
    printf("Claim not found.\n");
}

void displayMenu(int userIndex)
{
    printf("\nMenu:\n");
    if (users[userIndex].role == ROLE_ADMIN)
    {
        printf("1. Change User Role\n");
        printf("2. View Claims\n");
        printf("3. Modify Claim\n");
        printf("4. Delete Claim\n");
        printf("5. Exit\n");
    }
    else if (users[userIndex].role == ROLE_AGENT)
    {
        printf("1. View Claims\n");
        printf("2. Change Claim Status\n");
        printf("3. Exit\n");
    }
    else     // ROLE_CLIENT
    {
        printf("1. Submit Claim\n");
        printf("2. View Claims\n");
        printf("3. Exit\n");
    }
    printf("Choose an option: ");
}

int main()
{
    loadUsers();
    loadClaims();

    while (1)
    {
        printf("\n1. Sign Up\n2. Sign In\n3. Exit\n");
        int choice;
        printf("Choose an option: ");
        scanf("%d", &choice);
        getchar(); // Consume newline

        switch (choice)
        {
        case 1:
        {
            char username[MAX_USERNAME_LENGTH], password[MAX_PASSWORD_LENGTH];
            printf("Enter username: ");
            fgets(username, sizeof(username), stdin);
            username[strcspn(username, "\n")] = 0; // Remove newline

            printf("Enter password: ");
            fgets(password, sizeof(password), stdin);
            password[strcspn(password, "\n")] = 0; // Remove newline

            signUp(username, password);
            saveUsers(); // Save users after sign up
            break;
        }
        case 2:
        {
            char username[MAX_USERNAME_LENGTH], password[MAX_PASSWORD_LENGTH];
            printf("Enter username: ");
            fgets(username, sizeof(username), stdin);
            username[strcspn(username, "\n")] = 0; // Remove newline

            printf("Enter password: ");
            fgets(password, sizeof(password), stdin);
            password[strcspn(password, "\n")] = 0; // Remove newline

            int userIndex = signIn(username, password);
            if (userIndex >= 0)
            {
                while (1)
                {
                    displayMenu(userIndex);
                    int menuChoice;
                    scanf("%d", &menuChoice);
                    getchar(); // Consume newline

                    switch (menuChoice)
                    {
                    case 1:
                        if (users[userIndex].role == ROLE_ADMIN)
                        {
                            changeUserRole(userIndex);
                            saveUsers();
                        }
                        else if (users[userIndex].role == ROLE_AGENT)
                        {
                            viewClaims();
                        }
                        else     // ROLE_CLIENT
                        {
                            char claimDescription[MAX_CLAIM_DESCRIPTION];
                            printf("Enter claim description: ");
                            fgets(claimDescription, sizeof(claimDescription), stdin);
                            claimDescription[strcspn(claimDescription, "\n")] = 0; // Remove newline
                            submitClaim(users[userIndex].username, claimDescription);
                            saveClaims(); // Save claims after submission
                        }
                        break;
                    case 2:
                        if (users[userIndex].role == ROLE_ADMIN)
                        {
                            viewClaims();
                        }
                        else if (users[userIndex].role == ROLE_AGENT)
                        {
                            int claimId;
                            printf("Enter claim ID to change status: ");
                            scanf("%d", &claimId);
                            int newStatus;
                            printf("Enter new status (0: Pending, 1: In Progress, 2: Resolved, 3: Closed): ");
                            scanf("%d", &newStatus);
                            changeClaimStatus(claimId, (ClaimStatus)newStatus);
                            saveClaims(); // Save claims after changing status
                        }
                        else     // ROLE_CLIENT
                        {
                            viewClaims();
                        }
                        break;
                    case 3:
                        if (users[userIndex].role == ROLE_ADMIN)
                        {
                            int claimId;
                            char newDescription[MAX_CLAIM_DESCRIPTION];
                            printf("Enter claim ID to modify: ");
                            scanf("%d", &claimId);
                            getchar(); // Consume newline
                            printf("Enter new description: ");
                            fgets(newDescription, sizeof(newDescription), stdin);
                            newDescription[strcspn(newDescription, "\n")] = 0; // Remove newline
                            modifyClaim(claimId, newDescription);
                            saveClaims(); // Save claims after modification
                        }
                        else if (users[userIndex].role == ROLE_AGENT)
                        {
                            int claimId;
                            printf("Enter claim ID to delete: ");
                            scanf("%d", &claimId);
                            deleteClaim(claimId);
                            saveClaims(); // Save claims after deletion
                        }
                        break;
                    case 4:
                        printf("Exiting...\n");
                        break;
                    default:
                        printf("Invalid choice. Please try again.\n");
                    }

                    if (menuChoice == 5) break; // Exit the user menu
                }
            }
            break;
        }
        case 3:
            printf("Exiting program.\n");
            return 0;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    }
}
