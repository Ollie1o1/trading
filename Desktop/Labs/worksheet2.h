#define MAX_LEN_NAME 20
#define MAX_GRAD_STUDENTS 3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct justTag {
    
    char name [MAX_LEN_NAME];
    char supervisor [MAX_LEN_NAME];
    int numCoursesDone;
    float currentPercentage;
} gradStudent;

void printGrad (gradStudent);

int readGrads (FILE *file, gradStudent g [MAX_GRAD_STUDENTS]);

// write the prototype for oneMoreDone here


