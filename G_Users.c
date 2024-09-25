#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "G_Users.h"
#include "G_Claims.h"
#include <stdbool.h>




User users[MAX_USERS];
int userCount = 0;

void saveUsers()
{
    FILE *file = fopen(USER_FILE, "w");
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
    FILE *file = fopen(USER_FILE, "r");
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

int signIn(const char* username, const char* password)
{
    time_t currentTime = time(NULL);
    
    for (int i = 0; i < userCount; i++)
    {
        if (strcmp(users[i].username, username) == 0)
        {
            if (users[i].lockoutTime > currentTime)
            {
                printf("Account is locked. Please try again later.\n");
                return 0;
            }
            
            if (strcmp(users[i].password, password) == 0)
            {
                printf("Login successful. Welcome, %s!\n", username);
                users[i].loginAttempts = 0;
                return 1;
            }
            else
            {
                users[i].loginAttempts++;
                if (users[i].loginAttempts >= 3)
                {
                    users[i].lockoutTime = currentTime + LOCKOUT_DURATION;
                    printf("Too many failed attempts. Account locked for 5 minutes.\n");
                }
                else
                {
                    printf("Incorrect password. Attempts remaining: %d\n", 3 - users[i].loginAttempts);
                }
                return 0;
            }
        }
    }
    printf("User not found.\n");
    return 0;
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

    if (!isPasswordValid(password, username))
    {
        printf("Password validation failed. User not registered.\n");
        return 0;
    }

    strncpy(users[userCount].username, username, MAX_USERNAME_LENGTH - 1);
    users[userCount].username[MAX_USERNAME_LENGTH - 1] = '\0';

    strncpy(users[userCount].password, password, MAX_PASSWORD_LENGTH - 1);
    users[userCount].password[MAX_PASSWORD_LENGTH - 1] = '\0';

    users[userCount].role = (userCount == 0) ? ROLE_ADMIN : ROLE_CLIENT;
    users[userCount].loginAttempts = 0;
    users[userCount].lockoutTime = 0;

    printf("User registered successfully as %s.\n",
           (userCount == 0) ? "Admin" : "Client");
    userCount++;
    return 1;
}

void changeUserRole(UserRole userRole)
{
    if (userRole != ROLE_ADMIN)
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

User* authenticateUser() {
    static User currentUser;
    char choice;
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    do {
        printf("\n--- User Authentication ---\n");
        printf("1. Sign In\n");
        printf("2. Sign Up\n");
        printf("3. Exit\n");
        printf("Enter your choice (1-3): ");
        scanf(" %c", &choice);
        getchar(); // Consume newline

        switch(choice) {
            case '1':
                printf("Enter username: ");
                fgets(username, sizeof(username), stdin);
                username[strcspn(username, "\n")] = 0; // Remove newline

                printf("Enter password: ");
                fgets(password, sizeof(password), stdin);
                password[strcspn(password, "\n")] = 0; // Remove newline

                if (signIn(username, password)) {
                    for (int i = 0; i < userCount; i++) {
                        if (strcmp(users[i].username, username) == 0) {
                            currentUser = users[i];
                            return &currentUser;
                        }
                    }
                }
                break;

            case '2':
                printf("Enter new username: ");
                fgets(username, sizeof(username), stdin);
                username[strcspn(username, "\n")] = 0; // Remove newline

                printf("Enter new password: ");
                fgets(password, sizeof(password), stdin);
                password[strcspn(password, "\n")] = 0; // Remove newline

                if (signUp(username, password)) {
                    printf("Sign up successful. Please sign in.\n");
                }
                break;

            case '3':
                printf("Exiting program. Goodbye!\n");
                return NULL;

            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != '3');

    return NULL;  // This line is reached if the loop exits normally
}

void deleteUser(UserRole userRole) {
    if (userRole != ROLE_ADMIN) {
        printf("You do not have permission to delete users.\n");
        return;
    }

    char username[MAX_USERNAME_LENGTH];
    printf("Enter username to delete: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;  // Remove the newline character

    int userFound = 0;  // Flag to check if the user was found

    // Iterate through the users to find the one to delete
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            userFound = 1;

            // Shift all users after the deleted one to fill the gap
            for (int j = i; j < userCount - 1; j++) {
                users[j] = users[j + 1];
            }

            userCount--;  // Decrease user count
            printf("User '%s' has been deleted successfully.\n", username);
            break;
        }
    }

    if (!userFound) {
        printf("User '%s' not found.\n", username);
    }
}

void searchUser(UserRole userRole) {
    if (userRole != ROLE_ADMIN) {
        printf("You do not have permission to search for users.\n");
        return;
    }

    char username[MAX_USERNAME_LENGTH];
    printf("Enter username to search: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;  // Remove the newline character

    int userFound = 0;  // Flag to check if the user was found

    // Iterate through the users to find the one matching the search
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            userFound = 1;
            printf("User found:\n");
            printf("Username: %s\n", users[i].username);
            printf("Role: %s\n", (users[i].role == ROLE_ADMIN) ? "Admin" : (users[i].role == ROLE_AGENT) ? "Agent" : "Client");
            break;
        }
    }

    if (!userFound) {
        printf("User '%s' not found.\n", username);
    }
}
