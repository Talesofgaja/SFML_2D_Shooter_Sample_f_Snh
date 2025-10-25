#include <stdio.h>

int main()
{
    char name[50];
    int age;

    // Ask for the user's name
    printf("Enter your name: ");
    scanf("%s", name);

    // Ask for the user's age
    printf("Enter your age: ");
    scanf("%d", &age);

    // Print a personalized greeting and calculate the year they were born
    printf("Hello, %s!\n", name);
    printf("You are %d years old.\n", age);
    printf("You were likely born in %d.\n", 2025 - age);

    return 0;
}