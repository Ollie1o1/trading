#include <stdio.h>
#include "lab2A.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    Employees employees[NUM_EMP];
    int records = loadEmployees(employees, argv[1]);

    if (records > 0) {
        printf("Loaded %d records.\n", records);
        printf("First employee details:\n");
        printAnEmployee(employees[0]);
    } else {
        printf("No records loaded.\n");
    }

    return 0;
}