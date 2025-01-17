#include "worksheet2.h"

int main () {

    int numRead;
    gradStudent g [MAX_GRAD_STUDENTS];
    
    FILE * fptr = fopen ("file2.txt", "r");
    numRead = readGrads (fptr, g);
    
    for (int i = 0; i < numRead; i++) {
        printGrad (g[i]);
    }
    
    // call oneMoreDone for Harry Dang here - note that he is the first grad in g
   
    printGrad (g[0]);   // print Harry Dang's updated info
   
    return 0;
}

void printGrad (gradStudent g) {

   printf ("Name: %s\n", g.name);
   printf ("Supervisor: %s\n", g.supervisor);
   printf ("Number of Courses Done so far: %d\n", g.numCoursesDone);
   printf ("Current percentage: %.2f\n", g.currentPercentage);

}
