#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_USERS 100
#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define USER_FILE "users.dat"

typedef enum {
    ROLE_CLIENT,
    ROLE_AGENT,
    ROLE_ADMIN
} UserRole;

typedef struct {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    UserRole role;
    int loginAttempts;
    time_t lockoutTime;
} User;





User users[MAX_USERS];
int userCount = 0;




void saveUsers() {
    FILE *file = fopen(USER_FILE, "wb");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }
    fwrite(&userCount, sizeof(int), 1, file);
    fwrite(users, sizeof(User), userCount, file);
    fclose(file);
}



void loadUsers() {
    FILE *file = fopen(USER_FILE, "rb");
    if (file == NULL) {
        printf("No existing user data found.\n");
        return;
    }
    fread(&userCount, sizeof(int), 1, file);
    fread(users, sizeof(User), userCount, file);
    fclose(file);
}



int isPasswordValid(const char* password) {
    int hasUpper = 0, hasLower = 0, hasDigit = 0, hasSpecial = 0;
    for (int i = 0; password[i]; i++) {
        if (isupper(password[i])) hasUpper = 1;
        else if (islower(password[i])) hasLower = 1;
        else if (isdigit(password[i])) hasDigit = 1;
        else if (strchr("!@#$%^&*", password[i])) hasSpecial = 1;
    }
    return (strlen(password) >= 8 && hasUpper && hasLower && hasDigit && hasSpecial);
}

int signUp(const char* username, const char* password) {
    if (userCount >= MAX_USERS) {
        printf("Maximum user limit reached.\n");
        return 0;
    }

    if (strlen(username) == 0 || strlen(username) >= MAX_USERNAME_LENGTH) {
        printf("Invalid username length.\n");
        return 0;
    }

    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            printf("Username already exists.\n");
            return 0;
        }
    }

    if (!isPasswordValid(password)) {
        printf("Password does not meet security requirements.\n");
        return 0;
    }

    strcpy(users[userCount].username, username);
    strcpy(users[userCount].password, password);
    users[userCount].role = ROLE_CLIENT;  // Default role
    users[userCount].loginAttempts = 0;
    users[userCount].lockoutTime = 0;
    userCount++;

    saveUsers();  // Save updated user data to file

    printf("User registered successfully.\n");
    return 1;
}

int signIn(const char* username, const char* password) {
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            if (users[i].lockoutTime > 0 && difftime(time(NULL), users[i].lockoutTime) < 300) {
                printf("Account is locked. Try again later.\n");
                return 0;
            }

            if (strcmp(users[i].password, password) == 0) {
                users[i].loginAttempts = 0;
                users[i].lockoutTime = 0;
                saveUsers();  // Save updated login attempts
                printf("Login successful. Welcome, %s!\n", username);
                return 1;
            } else {
                users[i].loginAttempts++;
                if (users[i].loginAttempts >= 3) {
                    users[i].lockoutTime = time(NULL);
                    printf("Too many failed attempts. Account locked for 5 minutes.\n");
                } else {
                    printf("Incorrect password. Attempts left: %d\n", 3 - users[i].loginAttempts);
                }
                saveUsers();  // Save updated login attempts
                return 0;
            }
        }
    }
    printf("User not found.\n");
    return 0;
}

void setUserRole(const char* username, UserRole newRole) {
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            users[i].role = newRole;
            saveUsers();  // Save updated user role
            printf("Role updated for user %s.\n", username);
            return;
        }
    }
    printf("User not found.\n");
}

int main() {
    loadUsers();  // Load existing user data at program start

    // Example usage
    signUp("john_doe", "Passw0rd!");
    signUp("jane_smith", "Secur3Pass@");

    signIn("john_doe", "wrongpass");
    signIn("john_doe", "Passw0rd!");

    setUserRole("jane_smith", ROLE_AGENT);

    return 0;
}

