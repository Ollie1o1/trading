#include <stdio.h>
#include <string.h>
#include "lab2A.h"

int loadEmployees(Employees employees[], const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    int count = 0;
    while (count < NUM_EMP && fscanf(file, "%s %s %d %s %s %s", employees[count].fname, employees[count].lname,
                                      &employees[count].id,
                                      employees[count].dependents[0],
                                      employees[count].dependents[1],
                                      employees[count].dependents[2]) == 6) {
        count++;
    }

    fclose(file);
    return count;
}

void printAnEmployee(Employees employee) {
    printf("Id: %d\n", employee.id);
    printf("Full Name: %s %s\n", employee.fname, employee.lname);
    printf("Dependents: %s %s %s\n",
           employee.dependents[0],
           employee.dependents[1],
           employee.dependents[2]);
}