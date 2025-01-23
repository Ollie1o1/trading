#define LAB2A_H

#define NUM_EMP 10

typedef struct employee {
    char fname[20];
    char lname[20];
    int id;
    char dependents[3][20]; // Employees have exactly 3 registered dependents.
} Employees;

int loadEmployees(Employees employees[], const char* filename);
void printAnEmployee(Employees employee);
